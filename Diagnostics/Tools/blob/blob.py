import os
import bintool
import struct
import humanize
import subprocess
import re
import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom
import time

class ManifestEntry:
    def __init__(self, image_name="", image_version="", image_checksum=0, flags=0, hs_window=0, hs_lookahead=0, image_offset=0, image_size=0, path="", combine=False, chip=""):
        self.image_name = image_name.strip('\0')
        self.image_version = image_version.strip('\0')
        self.image_checksum = image_checksum
        self.flags = flags
        self.hs_window = hs_window
        self.hs_lookahead = hs_lookahead
        self.image_offset = image_offset
        self.image_size = image_size
        self.compress = True if hs_window and hs_lookahead else False
        self.path = path
        self.combine = combine
        self.chip = chip

    def disp(self):
        print "Manifest Entry"
        print "Image Name: %s" % self.image_name
        print "Image Version: %s" % self.image_version
        print "Image Offset: 0x%X" % self.image_offset
        print "Image Size: %d (%s)" % (self.image_size, humanize.naturalsize(self.image_size))
        if not self.flags == 0:
            print "Image Flags: 0x%X" % self.image_flags
        if not self.hs_lookahead == 0:
            print "Image HS Lookahead: %d" % self.hs_lookahead
        if not self.hs_window == 0:
            print "Image HS Window: %d" % self.hs_window
        print "compress: %r" % self.compress
        print ""

    @property
    def flags(self):
        return self.flags

    @property
    def hs_window(self):
        return self.hs_window

    @property
    def hs_lookahead(self):
        return self.hs_lookahead

    @property
    def image_version(self):
        return self.image_version

    @property
    def image_offset(self):
        return self.image_offset

    @image_offset.setter
    def image_offset(self, value):
        self.image_offset = value

    @property
    def image_size(self):
        return self.image_size

    @property
    def compress(self):
        return self.compress

    @property
    def image_checksum(self):
        return self.image_checksum

    @property
    def path(self):
        return self.path

    @property
    def combine(self):
        return self.combine

    @property
    def chip(self):
        return self.chip

    @image_checksum.setter
    def image_checksum(self, value):
        self.image_checksum = value

    def toSegment(self):
        image_name = self.image_name.ljust(24, '\0')
        image_version = self.image_version.ljust(24, '\0')
        return bintool.Segment.fromPack("< 24s 24s I H B B I I", image_name, image_version, self.image_checksum, int(self.flags), self.hs_window,
                                        self.hs_lookahead, self.image_offset, self.image_size)

    def calcPadding(self):
        return ((32 - (self.image_size) % 32)) % 32


class BlobHeader:
    def __init__(self, length=0, master_version="", num_entries=0):
        self.length = length
        self.master_version = master_version.strip('\0')
        self.num_entries = num_entries

    @property
    def length(self):
        return self.length

    @length.setter
    def length(self, value):
        self.length = value

    @property
    def master_version(self):
        return self.master_version

    @property
    def num_entries(self):
        return self.num_entries

    @num_entries.setter
    def num_entries(self, value):
        self.num_entries = value

    def disp(self):
        print "Blob:"
        print "Size: %d (%s)" % (self.length, humanize.naturalsize(self.length))
        print "Master Version: %s" % self.master_version
        print "Number of Images: %d" % self.num_entries
        print ""

    def toSegment(self):
        master_version = self.master_version.ljust(26, '\0')
        return bintool.Segment.fromPack("< I 26s H", self.length, master_version, self.num_entries)


class Blob:
    def __init__(self, timestamp=None):
        self.name = ""
        self.max_size = None
        self.header = None
        self.manifest_entries = []
        self.manifest_images = []
        self.checksum = 0
        self.heatshrink_path = "Tools/heatshrink-0.4.1/heatshrink"
        self.bos_file_name = ""
        if timestamp:
            self.timestamp = timestamp
        else:
            self.timestamp = (time.time() - 1357016400) / 60

    def add(self, path, name, version, hs_lookahead, hs_window, flags, combine, chip):
        if not os.path.isfile(path):
            raise Exception("File Not Found")
        if not self.header:
            raise Exception("Blob header invalid")

        manifest_image = bintool.readbin(path)

        #compress if necessary
        if hs_lookahead and hs_window:
            manifest_image = manifest_image.mapsegments(compress_segment_callback(self.heatshrink_path))

        manifest_entry = ManifestEntry(name, version, 0, flags, hs_window, hs_lookahead, 0, manifest_image.length(), path, combine, chip)
        self.manifest_entries.append(manifest_entry)
        self.manifest_images.append(manifest_image)
        self.header.num_entries += 1

        
    def load_from_file(self, path):
        if not os.path.isfile(path):
            raise Exception("File Not Found")

        self.name = os.path.basename(path)
        file_raw = bintool.readbin(path)
        header_raw = file_raw[0:32].moveto(0)
        manifest_raw = file_raw[32:-4].moveto(0)
        checksum_raw = file_raw[-4:].moveto(0)

        self.checksum = struct.unpack("< I", checksum_raw.segments[0].data)[0]
        calc_checksum = file_raw[:-4].checksum32(asSegment=False)

        # verify checksum
        if not self.checksum == calc_checksum:
            raise Exception("Invalid Blob Checksum")

        # parse header file
        self.header = BlobHeader(*struct.unpack("< I 26s H", header_raw.segments[0].data))

        # parse manifest table
        for i in range(0, self.header.num_entries):
            entry = manifest_raw[i * 64:(i + 1) * 64].moveto(0)
            manifest_entry = ManifestEntry(*struct.unpack("< 24s 24s I H B B I I", entry.segments[0].data))
            self.manifest_entries.append(manifest_entry)

            start_addr = 64 * self.header.num_entries + self.manifest_entries[i].image_offset
            end_addr = 64 * self.header.num_entries + self.manifest_entries[i].image_offset + self.manifest_entries[i].image_size
            size = (end_addr - start_addr)
            self.manifest_images.append(manifest_raw[start_addr:end_addr].moveto(0))
            calc_checksum = self.manifest_images[i].checksum32(asSegment=False)

            # verify image checksum
            if not self.manifest_entries[i].image_checksum == calc_checksum:
                raise Exception("Invalid Image Checksum")

                
    def load_from_manifest(self, path, build_type="Continuous", root_dir=".", exclude=[]):
        if not os.path.isfile(path):
            raise Exception("File Not Found")

        root = ET.parse(path).getroot()
        if not root.tag == "blob":
            raise Exception("Invalid XML File: must start with blob tag")

        for child in root:
            # header tag
            if child.tag == "header":
                # set name
                if 'name' in child.attrib:
                    self.name = child.attrib['name']
                else:
                    raise Exception("Invalid XML File: header tag missing 'name'")

                # set master_version
                if 'master_version' in child.attrib:
                    self.header = BlobHeader(master_version=child.attrib['master_version'])
                else:
                    raise Exception("Invalid XML File: header tag missing 'master_version'")

                if 'max_size' in child.attrib:
                    self.max_size = int(child.attrib['max_size'])

            # manifest_entries tag
            if child.tag == "manifest_entries":
                for entry in child:
                    # image_name
                    if 'name' in entry.attrib:
                        if entry.attrib['name'] in exclude:
                            continue
                        new_name = entry.attrib['name']
                    else:
                        raise Exception("Invalid XML File: 'entry' tag missing 'name' attribute")

                    # image
                    if 'path' in entry.attrib:
                        new_path = Blob.parse_path(entry.attrib['path'], path, build_type, root_dir)
                        print("%s using path = %s" % (new_name, new_path))

                    # image_version
                    if not 'version' in entry.attrib:
                        raise Exception("Invalid XML File: unable to find 'version' attribute")
                    new_image_version = None
                    if entry.attrib['version'] == 'dynamic':
                        # attempt to find properties file, and extract version number
                        m = re.search(r'(.*(?:Continuous|Nightly|Release)/[^/]+/[^/]+/)', new_path)
                        properties_file_name = None
                        if m is not None:
                            path_to_properties_file = m.groups()[0]
                            for file_name in os.listdir(m.groups()[0]):
                                properties_file_name = re.search(r'(Properties.*\.txt)', file_name) 
                                if properties_file_name is not None:
                                    with open(path_to_properties_file + properties_file_name.groups()[0]) as f:
                                        contents = f.read()
                                        m = re.search(r'[Vv]ersion[^\n]* = ([\d.]+)', contents)
                                        if m:
                                            new_image_version = m.groups()[0]
                        
                    if entry.attrib['version'] == 'filename' or (entry.attrib['version'] == 'dynamic' and new_image_version == None):
                        # attempt to grab version from the file name
                        for match in re.findall(r"[.\d]*\d", os.path.basename(new_path)):
                            if new_image_version is None  or len(match) > len(new_image_version):
                                new_image_version = match
                        if new_image_version is None:
                            raise Exception("Invalid XML File: unable to parse %s filename for version" % new_name)
                            
                    if new_image_version is None:
                        new_image_version = entry.attrib['version']
                        

                    # flags
                    if 'flags' in entry.attrib:
                        new_flags = entry.attrib['flags']
                    else:
                        new_flags = 0

                    # hs_window
                    if 'hs_window' in entry.attrib:
                        new_hs_window = int(entry.attrib['hs_window'])
                    else:
                        new_hs_window = 0

                    # hs_lookahead
                    if 'hs_lookahead' in entry.attrib:
                        new_hs_lookahead = int(entry.attrib['hs_lookahead'])
                    else:
                        new_hs_lookahead = 0

                    if 'combine' in entry.attrib:
                        combine = bool(entry.attrib['combine'])
                    else:
                        combine = False

                    if 'chip' in entry.attrib:
                        chip = entry.attrib['chip']
                    else:
                        chip = ""

                    self.add(new_path, new_name, new_image_version, new_hs_lookahead, new_hs_window, new_flags, combine, chip)

                    
    def recalculate(self):
        if len(self.manifest_entries)!= len(self.manifest_images) or len(self.manifest_entries) != self.header.num_entries:
            raise Exception("blob entries number mismatch")

        # recalculate image offsets
        image_offset = 0
        for i in range(0, self.header.num_entries):
            self.manifest_entries[i].image_offset = image_offset
            image_offset += self.manifest_entries[i].image_size
            # padding
            image_offset += self.manifest_entries[i].calcPadding()
            # checksum
            checksum = self.manifest_images[i].checksum32(asSegment=False)
            self.manifest_entries[i].image_checksum = checksum

        # recalcluate total blob size
        # header = 32
        # manfiest_entires = 64 per entry
        # images = sizeof(manfiest_images)
        # checksum = 4
        self.header.length = 32 + (64 * self.header.num_entries) + image_offset + 4


    def pack(self, path):
        self.recalculate()
        sb = bintool.SparseBuffer()
        # Append timestamp to version and add header
        self.header.master_version += "." + str(int(self.timestamp))
        header_segment = self.header.toSegment()
        sb = sb.append(header_segment)
        # manifest_entries
        for i in range(0, self.header.num_entries):
            manifest_entry = self.manifest_entries[i].toSegment()
            sb = sb.append(manifest_entry)
        # manifest_images
        for i in range(0, self.header.num_entries):
            # image
            manifest_image = self.manifest_images[i].segments[0]
            sb = sb.append(manifest_image)
            # image padding
            padding_array = [0x00] * self.manifest_entries[i].calcPadding()
            padding = bytes().join((struct.pack("< B", int(val)) for val in padding_array))
            padding_image = bintool.Segment(0, padding)
            sb = sb.append(padding_image)

        # checksum
        checksum_segment = sb.checksum32(fmt="<")
        sb = sb.append(checksum_segment)
        self.checksum = sb.checksum32(fmt="<", asSegment=False)

        if self.max_size and self.header.length > self.max_size:
            raise Exception("Blob too large")
        # save file
        self.bos_file_name = os.path.splitext(self.name)[0] + '_' + self.header.master_version + '.bos'
        sb.writebin(os.path.join(path, self.bos_file_name))

    def unpack(self, name, output_dir):
        self.name = name
        self.recalculate()
        for i in range(0, self.header.num_entries):
            manifest_image = self.manifest_images[i]
            if self.manifest_entries[i].compress:
                manifest_image = manifest_image.mapsegments(decompress_segment_callback(self.heatshrink_path))
            output_file = os.path.join(output_dir, self.manifest_entries[i].image_name.decode('utf8'))
            manifest_image.writebin(output_file)
        self.create_manifest(os.path.join(output_dir, "manifest.xml"))


    def disp(self):
        self.recalculate()
        self.header.disp()
        for i in range(0, self.header.num_entries):
            self.manifest_entries[i].disp()


    def create_manifest(self, output_file):
        blob = ET.Element("blob")
        blob.insert(0, ET.Comment(Blob.manifest_comment()))
        ET.SubElement(blob, "header",
                      name=self.name,
                      master_version=self.header.master_version)
        manifest_entries = ET.SubElement(blob, "manifest_entries")
        for i in range(0, self.header.num_entries):
            manifest_entry = self.manifest_entries[i]
            entry = ET.SubElement(manifest_entries, "entry",
                                  name=manifest_entry.image_name,
                                  version=manifest_entry.image_version,
                                  path=os.path.join(".", manifest_entry.image_name))
            test = str(manifest_entry.image_name).strip('\0')
            if not manifest_entry.flags == 0:
                entry.attrib["flags"] = str(manifest_entry.flags)
            if manifest_entry.compress:
                entry.attrib["hs_lookahead"] = str(manifest_entry.hs_lookahead)
                entry.attrib["hs_window"] = str(manifest_entry.hs_window)
                

        xml_str = minidom.parseString(ET.tostring(blob)).toprettyxml(indent="    ")
        with open(output_file, "w") as f:
            f.write(xml_str)


    def create_configuration(self, output_file):
        config = ET.Element("config")
        ET.SubElement(config, "header",
                      name=self.name,
                      master_version=self.header.master_version)

        ET.SubElement(config, "checksum", value=str(self.checksum))
        ET.SubElement(config, "filename", name=self.bos_file_name)

        manifest_entries = ET.SubElement(config, "manifest_entries")
        for i in range(0, self.header.num_entries):
            manifest_entry = self.manifest_entries[i]
            entry = ET.SubElement(manifest_entries, "entry",
                                  name=manifest_entry.image_name,
                                  version=manifest_entry.image_version,
                                  path=os.path.join(".", manifest_entry.image_name))
            test = str(manifest_entry.image_name).strip('\0')
            if not manifest_entry.flags == 0:
                entry.attrib["flags"] = str(manifest_entry.flags)
            if manifest_entry.compress:
                entry.attrib["hs_lookahead"] = str(manifest_entry.hs_lookahead)
                entry.attrib["hs_window"] = str(manifest_entry.hs_window)

        xml_str = minidom.parseString(ET.tostring(config)).toprettyxml(indent="    ")
        with open(output_file, "w") as f:
            f.write(xml_str)


    def update_manifest(self, manifest_path, name=None, version=None):
        if name is None and version is None:
            return None
        if not os.path.isfile(manifest_path):
            raise Exception("File Not Found")

        tree = ET.parse(manifest_path)
        root = tree.getroot()
        if not root.tag == "blob":
            raise Exception("Invalid XML File: Root tag in file must be <blob>")

        #Comments are not parsed, add the comment in again
        root.insert(0, ET.Comment(Blob.manifest_comment()))
        header = root.find("header")
        if header is None:
            raise Exception("Invalid XML File: cannot find \"header\" tag")
        if name is not None:
            header.attrib["name"] = name
        if version is not None:
            header.attrib["master_version"] = version
        tree.write(manifest_path)

    def create_combined_hex(self, output_dir):
        arg_strings = {}
        for entry in self.manifest_entries:
            if entry.combine:
                if not entry.chip:
                    raise Exception("The name of the chip must be set if a combined image is desired")
                if entry.chip not in arg_strings.keys():
                    arg_strings[entry.chip] = []
                arg_strings[entry.chip].append(entry.path.replace(".bin", ".hex"))

        for chip in arg_strings.keys():
            command = "srec_cat "
            for arg in arg_strings[chip]:
                command += "%s -Intel " % arg

            command += "-o %s_combined_%s.hex -Intel " % (os.path.join(output_dir, chip), self.header.master_version)

            print command
            ret = subprocess.Popen(command, shell=True).wait()
            if ret:
                raise Exception("Could not create combined image for %s" % chip)

    # A comment, inserted into constructed XML manifests
    @staticmethod
    def manifest_comment():
        return """
        *   To cause an image to be compressed, provide a nonzero hs_window and/or
            hs_lookahead. If not included, 0 (not compressed) will be assumed.

        *   'version' may be a hardcoded version, 'dynamic' or 'filename'. If
            version=filename the script will use the longest string of numbers
            and periods in the file name as the version number. If
            version=dynamic a properties file is searched for and parsed, and
            if that fails it falls back to the 'filename' approach.

        *   *ROOT* may be used in the path attribute. It will be resolved to the root
            directory of the project, provided to the script.
        
        *   *CNR* may be used in the path attribute. It will be resolved as "Continuous",
            "Nightly", or "Release", based on parameters passed to the script.

        *   The 'path' attribute may consist of multiple paths separated by commas.
            The first of these paths to match to a real file will be used.
        
        *   The string "REGEX:" at the beginning of a path will cause the file name
            portion to be treated like a regex, and matched against files in the
            directory. If REGEX: is not used, an asterisk (*) can still be used as a
            wildcard to match characters. If either of these methods match more than one
            file, the program will exit with an error.
            
            """


    # parse path from xml file, return a valid path if possible
    @staticmethod
    def parse_path(path, manifest_path, build_type, root_dir):
        for p in path.split(','):
            p = re.sub(r"\*CNR\*", build_type, p)
            p = re.sub(r"\*ROOT\*", root_dir, p) 

            uses_regex = re.match(r'REGEX:(.*)', p)
            if uses_regex:
                p = uses_regex.groups()[0]
            
            if not os.path.isabs(p):
                p = os.path.join(os.path.dirname(manifest_path), p)

            if uses_regex:
                # Treat filename as a regex
                path_regex = os.path.basename(p)
                matching_file = None
                for file_name in os.listdir(os.path.dirname(p)):
                    n = re.search(path_regex, file_name)
                    if n and matching_file is not None:
                        raise Exception("Invalid XML File: regex matches multiple files (%s)" % entry.attrib['path'])
                    if n:
                        matching_file = file_name
                p = os.path.join(os.path.dirname(p), matching_file)
                    
            else:
                # look for '*' wildcard in the filename
                res = re.search(r"/([^/]*)\*([^/]*)$", p)
                if res:
                    l, r = res.group(1, 2)
                    new_regex = l + r"[^/]*" + r
                    matching_file = None
                    for file_name in os.listdir(os.path.dirname(p)):
                        matches = re.search(new_regex, file_name)
                        if matches and matching_file is not None:
                            raise Exception("Invalid XML File: wildcard matches multiple files")
                        if matches:
                            matching_file = file_name
                    if matching_file is not None:
                        p = os.path.join(os.path.dirname(p), matching_file)

            if os.path.isfile(p):
                return p
        else:
            raise Exception("Invalid XML File: invalid path (%s) provided" % path)



# callbacks for bintool.SparseBuffer.mapsegments
def compress_segment_callback(heatshrink_path, hs_window=11, hs_lookahead=4):
    def compress_segment(segment):
        p = subprocess.Popen([heatshrink_path, "-e", "-w", str(hs_window), "-l", str(hs_lookahead)], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        compressed = p.communicate(segment.data)[0]
        return bintool.Segment(0, compressed)
    return compress_segment

def decompress_segment_callback(heatshrink_path):
    def decompress_segment(segment):
        p = subprocess.Popen([heatshrink_path, "-d"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        compressed = p.communicate(segment.data)[0]
        return bintool.Segment(0, compressed)
    return decompress_segment
