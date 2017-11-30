"""

Given a list of manifest files, iterate over each compressed element
and find the best hs_window and hs_lookahead. Note that this may take
a long time with a large window/lookahead range, or a large number
of compressed files.

"""
import os
import subprocess
import argparse
import xml.etree.ElementTree as ET

# import blob, to get Blob.manifest_comment() and parse_path()
import sys
sys.path.insert(0, "..")
import blob

parser = argparse.ArgumentParser()
parser.add_argument('Files', nargs = '+', help = "Manifest files")
parser.add_argument('--update', action="store_true", help="Edit manifest files with the updated heatshrink params.")
args = parser.parse_args()

update = True if args.update else False

for manifest_path in args.Files:
    if not os.path.isfile(manifest_path):
        print("Invalid path: %s" % manifest_path)
        continue
    tree = ET.parse(manifest_path)
    root = tree.getroot()
    root.insert(0, ET.Comment(blob.Blob.manifest_comment()))
    manifest_entries = root.find("manifest_entries")
    if manifest_entries is None:
        print("Cannot find <manifest_entries> element in manifest \"%s\"" % manifest_path)
        continue
    for entry in manifest_entries:
        if 'hs_window' in entry.attrib and entry.attrib['hs_window'] != '0' and 'hs_lookahead' in entry.attrib and entry.attrib['hs_lookahead'] != 0:
            # Now, do an analysis on the file represented by the entry
            path = entry.attrib['path']
            path = blob.Blob.parse_path(path, manifest_path, "Continuous", "../..")
            if not os.path.isfile(path):
                continue
            print("\nTesting file %s ..." % path)
            smallest = (0, 0, 0)
            for window in range(6, 13):
                for lookahead in range(3, 7):
                    subprocess.call(["../../heatshrink-0.4.1/heatshrink", "-w", str(window), "-l", str(lookahead), path, "HSOUT"])
                    size = os.stat("HSOUT").st_size
                    if size > 0:
                        print("window=%d and lookahead=%d produces a %d byte file" % (window, lookahead, size))
                    if smallest[2] == 0 or (size > 0 and size < smallest[2]):
                        smallest = (window, lookahead, size)
                    os.remove("HSOUT")

            print("%s should use window=%d, lookahead=%s, for a compressed size of %d bytes" % (os.path.basename(path), smallest[0], smallest[1], smallest[2]))
            if update:
                print("Updating XML file with new values")
                entry.set('hs_window', str(smallest[0]))
                entry.set('hs_lookahead', str(smallest[1]))
                tree.write(manifest_path)