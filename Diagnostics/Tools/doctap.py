import os
import re
import operator
import argparse
from xml.sax.saxutils import escape

'''
python tool to build html from doctap sections of TAP code

Scans .c and .h files looking for sections enclosed by @DOCETAP and @DOCETAPEND
Looks for and parses command denoted by tag "COMMAND:" and alias denoted by "ALIAS:"
Will capture the rest of text and put this in html file.
Will appear exactly as written.

suggested format for building DOCTAP sections:

@DOCETAP

COMMAND:

ALIAS:

PARAM:

REPLY:

SUBCOMMAND:     (if any)

EXAMPLE:

@DOCETAPEND


This python file is set to be callable from the command line as follows:

Python doctap.py html_title file_search_path html_output_path

'''

class TapDocBuilder:
    directory_filter = "Tap"
    ignored_dir = "ExampleProject"

    def __init__(self, name, input_path, output_path):
        self.current_dir = os.getcwd()
        self.html_out_name = name
        self.input_path = input_path
        self.output_path = output_path

    def parse(self):
        tap_doc_list = []
        for root, dirs, files in os.walk(self.input_path):
            if self.directory_filter in root and self.ignored_dir not in root:
                tap_doc_list.extend(self.create_doctap_dictionary(root))

        return tap_doc_list

    def build_HTML(self, doctap_dictionary):

        doctap_dictionary.sort(key=lambda x: x['COMMAND'].lower())  # sort dict by command ID alphabetically
                                                                    # and case insensitive

        HTML_file = open(os.path.join(self.current_dir, self.html_out_name) + ".html", 'w')  # create/update file
        HTML_file.write("<html><head><title>" + self.html_out_name + "</title>\n")
        HTML_file.write("</head><body>\n")
        HTML_file.write("<font face=\"courier\">\n")
        HTML_file.write("<h1>" + self.html_out_name + "</h1><ul>\n")

        for i in range(0, len(doctap_dictionary)):
            link = doctap_dictionary[i]['COMMAND'] + " [" + doctap_dictionary[i]['ALIAS'] + "]"
            link = link.replace('\n', '').replace('\r', '')
            HTML_file.write("<a href=\"#" + link + "\"><li>" + link + "</li></a>\n")

        HTML_file.write("</ul><br><hr>\n")

        for i in range(0, len(doctap_dictionary)):
            link = doctap_dictionary[i]['COMMAND'] + " [" + doctap_dictionary[i]['ALIAS'] + "]"
            link = link.replace('\n', '').replace('\r', '')
            HTML_file.write("<a name= \"" + link + "\"></a>")
            HTML_file.write("<pre>")

            HTML_file.write(escape(doctap_dictionary[i]['DESC']))
            HTML_file.write("</pre><hr>\n\n")

        HTML_file.write("</body>\n</html>")

        HTML_file.close()

    def test_doctap(self,string, current_file):
        #test format of doctap to ensure validity
        #assumes it has COMMAND and ALIAS (because of where I will place this function)

        numR = 0
        numP = 0
        numS = 0
        search_descrip = re.search("DESCRIP:\s+", string)
        if not search_descrip:
            print "Warning: missing 'DESCRIP' field found in %s" %current_file

        search_param = re.search("PARAM:\s+", string)
        if not search_param:
           print "Warning: missing 'PARAM' field found in %s" %current_file
        else:
            params = re.findall("PARAM:\s+", string)
            numP = len(params)


        search_reply = re.search("REPLY:\s+", string)
        if not search_reply:
            print "Warning: missing 'REPLY' field found in %s" %current_file
        else:
            replys = re.findall("REPLY:\s+", string)
            numR = len(replys)

        search_sub = re.search("SUBCOMMAND:\s+", string)
        if search_sub:
            subs = re.findall("SUBCOMMAND:\s+", string)
            numS = len(subs)

            if not ((numS == numP and numS == numR) or (numS == numP + 1 and numS == numR + 1)):
                print "Warning: check # of subcommands, parameters, and replys in %s" % current_file

    def create_doctap_dictionary(self, tap_dir):
        # creates list of dictionaries (COMMAND, ALIAS, desc(all text))

        master_list = []
        file_list = [os.path.join(tap_dir, file) for file in os.listdir(tap_dir)]
        for current_file in file_list:
            if os.path.isdir(current_file):
                continue
            temp_list = self.sift_for_doctap(current_file)           # finds all enclosed doctaps within file
            if temp_list:
                # proceed to finding COMMAND, ALIAS, desc
                for i in range(0, len(temp_list)):
                    command_line = re.search('COMMAND:\s+', temp_list[i])  # find command tag
                    if command_line:
                        command_line_text = temp_list[i][command_line.span()[1]:]  # get text
                        command_find = re.search('.+\n', command_line_text)  # find command name
                        if command_find:
                            command = command_line_text[command_find.span()[0]:command_find.span()[1] - 1]
                            alias_line = re.search('ALIAS:\s+', temp_list[i])
                            if alias_line:
                                alias_line_text = temp_list[i][alias_line.span()[1]:]
                                alias_find = re.search('.+\n', alias_line_text)    #find alias name
                                if alias_find:
                                    alias = alias_line_text[alias_find.span()[0]:alias_find.span()[1]-1]

                                    #place test for 'good doc' function here
                                    #self.test_doctap(temp_list[i], current_file)

                                    desc = re.sub('\*', ' ', temp_list[i])
                                    dict = {'COMMAND': command, 'ALIAS': alias, 'DESC': desc}
                                    master_list.append(dict)
                                else:
                                    print "Error: Missing alias in %s" % current_file
                                    print "Doctap skipped!\n"
                            else:
                                print "Error: Missing alias in %s" % current_file
                                print "Doctap skipped!\n"
                        else:
                            print "Error: Missing command in %s" % current_file
                            print "Doctap skipped!\n"
                    else:
                        print "Error: Missing command in %s" % current_file
                        print "Doctap skipped!\n"



        return master_list

    def sift_for_doctap(self, file):
        #finds enclosed doctaps within 'file'
        #reports errors if found

        file_object = open(file, 'r')  # open file
        text = file_object.readlines()  # get text in form of a list by line no.

        doc_list = []

        docStart = 0  # used as flag and index holder of where @DOCETAP
        docEnd = 0  # and @DOCETAPEND are located, respectively

        for line in range(0, len(text)):
            index1 = re.search("@DOCETAP\s", text[line])
            index2 = re.search("@DOCETAPEND\s", text[line])
            if index1:
                if docStart:
                    print "Error: File %s Line %d. Missing @DOCETAPEND. Rest of file skipped!\n" % (file, line + 1)
                    return doc_list
                else:
                    docStart = line  # flag for @DOCETAP
            elif index2:
                if docStart:
                    docEnd = line
                else:
                    print "Error: File %s Line %d. Found @DOCETAPEND before @DOCETAP. Rest of file skipped!\n" % (\
                    file, line + 1)
                    return doc_list

            if (docStart and docEnd):
                temp = ''
                for i in range(docStart + 1, docEnd - 1):
                    temp += text[i]  # get text

                doc_list.append(temp)  # append to doc_list

                docStart = 0  # reset flag/indices
                docEnd = 0  #

        return doc_list

    def run(self):
        tap_list = self.parse()
        self.build_HTML(tap_list)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("name")
    parser.add_argument("project_root_dir")
    parser.add_argument("html_output_directory")

    args = parser.parse_args()
    tap_build = TapDocBuilder(args.name, args.project_root_dir, args.html_output_directory)

    tap_build.run()
