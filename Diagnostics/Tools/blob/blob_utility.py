import argparse
import blob
import os

parser = argparse.ArgumentParser()
parser.add_argument('--info', nargs=1, help="Print info about blob")
parser.add_argument('--pack', nargs=2, help="Use an XML manifest to create a blob")
parser.add_argument('--build_type', nargs=1, help="Continuous, Nightly, or Release replacement when parsing XML")
parser.add_argument('--version', nargs=1, help="Update blob version in XML header while packing")
parser.add_argument('--name', nargs=1, help="Update blob name in XML header while packing")
parser.add_argument('--unpack', nargs=2, help="Unpack an existing blob into it's files and XML")
parser.add_argument('--root', nargs=1, help="Root directory of the project. Use absolute path, or relative path to XML file.")
parser.add_argument('--exclude', nargs='*', help="Names of XML entries to ignore when parsing.")
parser.add_argument('--timestamp', help="Sets the timestanp to be used when naming the output.")
args = parser.parse_args()

if args.info is not None:
    b = blob.Blob()
    b.load_from_file(args.info[0])
    b.disp()

if args.unpack is not None:
    blob_path = args.unpack[0]
    blob_output_dir = args.unpack[1]

    if not os.path.exists(blob_output_dir):
        os.makedirs(blob_output_dir)

    b = blob.Blob()
    b.load_from_file(blob_path)
    b.unpack(os.path.basename(blob_path), blob_output_dir)

if args.pack is not None:
    manifest_path = args.pack[0]
    blob_output_dir = args.pack[1]

    if not os.path.exists(blob_output_dir):
        os.makedirs(blob_output_dir)

    timestamp = args.timestamp
    b = blob.Blob(timestamp)

    new_name = args.name[0] if args.name else None
    new_ver = args.version[0] if args.version else None
    if new_name or new_ver:
        b.update_manifest(manifest_path, new_name, new_ver)

    if args.build_type:
        if args.build_type[0] == "Continuous" or args.build_type[0] == "Nightly" or args.build_type[0] == "Release":
            build_type = args.build_type[0]
        else:
            raise Exception("Must provide a build type of continuous, nightly, or release")
    else:
        build_type = "Continuous"
        
    if args.root:
        root_dir = args.root[0]
    else:
        root_dir = '.'

    exclude = args.exclude if args.exclude is not None else []

    b.load_from_manifest(manifest_path, build_type=build_type, root_dir=root_dir, exclude=exclude)
    b.pack(blob_output_dir)
    b.create_combined_hex(blob_output_dir)
    b.create_configuration(os.path.join(blob_output_dir, 'lpm_blob_configuration.xml'))
