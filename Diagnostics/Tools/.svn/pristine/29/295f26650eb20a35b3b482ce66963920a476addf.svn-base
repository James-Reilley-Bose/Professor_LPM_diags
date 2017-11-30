#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:     COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
#                 This program may not be reproduced, in whole or in part in any
#                 form or any means whatsoever without the written permission of:
#                     BOSE CORPORATION
#                     The Mountain,
#                     Framingham, MA 01701-9168
#
"""
   TODO:  Extend this file to update several components of the device attached by TAP.
   This module provides and quick, easy and inflexible way of updating devices connected
   by TAP.  Currently it only works on Windows.
   It does not provide any useful classes for external use.
"""
import os
import sys
import time

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir, os.pardir)))

import serial
import argparse

class TapSerialUpdate(object):

    def __init__(self, comport, device="LPM", prompt="A4V>", baudrateArg=115200, parityArg=serial.PARITY_NONE,
                 stopbitsArg=serial.STOPBITS_ONE, bytesizeArg=serial.EIGHTBITS):

        self.port_number = comport
        self.term_char = "\r"
        self.device_type = device
        self.cmd_prompt = prompt

        self.Coms_port = serial.Serial(port=self.port_number,
                                       baudrate=baudrateArg,
                                       parity=parityArg,
                                       stopbits=stopbitsArg,
                                       bytesize=bytesizeArg,
                                       timeout=0)
        serial.Serial()

    def OpenPort(self):
        if(self.Coms_port.isOpen() != True):
            try:
                self.Coms_port.open()
                print("opening - serial port:{}".format(self.port_number))
            except Exception, e:
                print("error open serial port: {}".format(self.port_number + str(e)))

    def __del__(self):
        self.Coms_port.close()

    def ClosePort(self):
        self.Coms_port.close()

    # terminating string
    # string to check
    def SendCmdReadMulti(self, command, timeout_s=3, wait_resp=None):
        """
        Sends a tap command and waits timeout_s as it waits for wait_resp
        :param command:
        :param timeout_s: number of seconds to continue reading while waiting for wait_resp
          NOTE: this is before wait_resp as wait_resp was added later
        :param wait_resp: string/prompt to wait for. the default is the command
        :return: response string up to wait_resp or "" if nothing is read
        """
        if wait_resp is None:
            wait_resp = self.cmd_prompt

        self.Coms_port.flush()
        self.Coms_port.flushOutput()

        self.send_cmd(command)
        response_string = self.wait_for_resp(wait_resp, timeout_s)
        return response_string

    def wait_for_resp(self, exp_resp=None, timeout_s=3):
        """
        method only reads from tap for until timeout_s or exp_resp is found
        :param exp_resp: expected response to read until it is found
        :param timeout_s: seconds to continue search
        :return: returns the string found in tap or empty string
        """

        timeout = time.time() + timeout_s   # timeout in seconds from now

        if exp_resp is None:
            exp_resp = self.cmd_prompt

        output_characters = []
        response_string = ""
        while 1:
            if time.time() < timeout:
                if self.Coms_port.inWaiting():
                    output_characters += self.Coms_port.read()
                    response_string = ''.join(output_characters)
                    if exp_resp in response_string:
                        break
            else:
                # print("Timeout - Did not find response: {} in Tap response: ({}) ".format(exp_resp, response_string))
                print("Timeout - Did not find response: {} in Tap response: {} ".format(exp_resp, response_string))
                break

        # print("TAP response: {} ".format(response_string))
        return response_string

    def send_cmd(self, command):
        """
        Write to the serial bus.
        :param command: what you would like to send -> term_char added.
        :return: None
        """
        # clear any excess garbage off the input buffer
        self.Coms_port.flush()
        self.Coms_port.flushOutput()

        cmd = command + self.term_char
        print("Command is: {}".format(cmd))
        self.write_with_char_delay(cmd, 1)  # Send command!

    def write_with_char_delay(self, cmd, dly_ms=0):
        """
        Write to the serial bus with a delay between each character.This is a work around for
        PAELLA-2284, an interrupt scheduling issue with the LPM when tap commands are sent too quickly.
        :param cmd: what you would like to send -> term_char added.
        :param dly_ms: delay in msec
        :return: None
        """

        if dly_ms == 0:
            self.Coms_port.write(cmd)  # Send command!
        else:
            dly_ms *= .001  # convert to sec
            for ch in cmd:
                self.Coms_port.write(ch)  # Send command!
                time.sleep(dly_ms)

    def read_line(self, line_terminator="\n", timeout_seconds = 2):
        """
        Read a single line from the serial connection.
        Will read until the end of line is found OR the prompt is found.
        The prompt will not contain an end of line thus both is needed.
        :param line_terminator: end of line terminator or terminators
        :return: a single line
        """
        keep_reading = True
        output_characters = []
        response_string = ""
        timeout = time.time() + timeout_seconds
        while keep_reading:
            if time.time() < timeout:
                if self.Coms_port.inWaiting():
                    output_characters += self.Coms_port.read()
                    response_string = ''.join(output_characters)
                    if line_terminator in response_string or self.cmd_prompt in response_string:
                        keep_reading = False
            else:
                response_string += "ERROR - Timeout; no end of line found:"
                print(response_string)
                keep_reading = False

        return repr(response_string)


class LpmStandaloneUpdater(object):
    """
    LpmStandaloneUpdater class to handle the high level aspects to update the device.  The LpmStandaloneUpdater object
    holds the information about the update itself as well as the TAP connection to the device being updated.
    """

    # up_type_dict provides the starting addresses for each of the components to load into memory
    up_type_dict = {"user": "0x8060000", "diags": "0x8020000", "blob": "0x00000000", "speaker": "0x8040000", "anim": "0x003C0000"}

    # so its global make default
    location = None

    def __init__(self, com_port = None, file_name = "", path="", filetype="user"):
        """
        Initialize this LpmStandaloneUpdater object.  Assumes that all necessary information is present and
         is correct.  Exits on any errors.
        :param com_port: serial port of the TAP connected device
        :param file_name: points to the .bos file containing the update
        :param path: directory of the update .bos file or empty if in current directory
        :param filetype: type of update in the update file
        """

        # Open the com port to the A4V system.
        # Exit if no port is set.
        if com_port is None:
            print("ERROR Com port not set")
            sys.exit(1)
        else:
            self.tap = TapSerialUpdate(com_port, "LPM", "LPM>", '115200')

        # Set Flash memory location for this type of update, exit if unrecognized
        try:
            self.location = self.up_type_dict[filetype]
        except Exception as e:
            print("Invalid update type")
            sys.exit(1)

        # Check to see if we got a path to where the file lives, otherwise use current directory.
        if path:
            self.file_path = path +'\\'+ file_name
        else:
            self.file_path = os.path.abspath(os.path.join(os.path.dirname(__file__))) +'\\'+ file_name

        if not os.path.exists(self.file_path):
            print("Error file: {:s} \n!!Does not exist please double check the path and the filename".format(self.file_path))
            sys.exit(1)

        print("\n\n!!!!Found update file and path: {:s}".format(self.file_path))
        if 'exit' in raw_input("Hit ENTER to CONTINUE with update, otherwise type 'exit' to end script!").lower():
            print("Ending script")
            sys.exit()


    def update(self):
        """
        This method takes the information input by the user and calls the lower level method to send the data to
        the device.  It allows the user to specify which components to apply the update to once the data has been
        passed to the TAP connected device.
        :return: None
        """

        if self.location == None:
            print("Invalid locations")
            return

        file_size = os.stat(self.file_path).st_size
        file_size_hex = hex(file_size)

        # Get into boot> mode:
        resp = self.tap.SendCmdReadMulti("\n", 3, ">")
        if "boot>" not in resp:
            self.tap.SendCmdReadMulti("rb 2", 10, "boot>")


        send_cmd = "tapload " + self.location + ", " + file_size_hex + ", 0, 256"

        resp = self.tap.SendCmdReadMulti(send_cmd, 2, "Erasing external flash storage...")
        resp = self.tap.wait_for_resp("Please send file...", 90)

        # Count the number of loops through this loop.  Every 200 iterations tell the user what
        # percent of the file has been uploaded.  The actual count is not important; it's only
        # used as a way to get the percent complete displayed.
        loop_count = 0
        with open(self.file_path, "rb") as f:
            data = f.read(256)

            while data != b"":
                # Send 256 Bytes Via Serial
                num_bytes_sent = str(len(data))
                self.tap.Coms_port.write(data)
                resp = self.tap.wait_for_resp("...", 5)

                if ":" not in resp:
                    print("Error was not able to verify that the bootloader is receiving bytes")

                num_bytes_rec = resp.split(':')[1].strip()
                if num_bytes_sent != num_bytes_rec:
                    print("Sent {} bytes, received {}, but this is fine.".format(num_bytes_sent, num_bytes_rec))

                # Every 200 times through this loop inform the user that we're still running and how far we've gone
                # as a percent of the total upload size.
                if (loop_count % 200) is 0:
                    print("Transferred {:.1%}".format(float(256 * loop_count) / float(file_size)))

                # Bottom of the loop, update counter and data buffer
                loop_count += 1
                data = f.read(256)

        time.sleep(1)
        resp = resp + self.tap.wait_for_resp("...", 5)
        num_total_bytes_rec = resp.split()[1].split(':')[1]
        if file_size != int(num_total_bytes_rec):
            print(file_size, num_total_bytes_rec, "Error total sent bytes " + str(file_size) +
                          " does not match bytes received " + str(num_total_bytes_rec))
            print("{} {} Error total sent bytes {} does not match bytes received {}"
                  .format(file_size, num_total_bytes_rec, str(file_size), str(num_total_bytes_rec)))

        # After sending bardeen___.bos you can send "up lpm" at the boot loader.
        # Finally boot to App
        # self.tap.
        resp = self.tap.SendCmdReadMulti("boot 0", 5)
        if 'A4V>' not in resp:
            print('Sent "boot 0" and got {}'.format(resp))
            resp = self.tap.SendCmdReadMulti('boot', 5)
            if 'A4V>' not in resp:
                print('Sent "boot" and got {}'.format(resp))
                print('Cannot find correct A4V> response.  Exiting.')
                self.tap.ClosePort()
                sys.exit(1)
        print("\n\nNow if you want you can type 'up lpm', 'up hdmi', 'up dsp' to update each component" )
        print('Enter your commands below.\r\nType "bye" to leave the application.')

        # Allow user to send commands to update specific components; no checks for correctness
        updating = True
        while updating:
                input = raw_input("")
                if input.lower() == 'bye':
                    updating = False
                    self.tap.ClosePort()
                else:
                    if "up" in input:
                        self.tap.send_cmd(input)

                        output_characters = []
                        response_string = ""
                        start_time = time.time()
                        while 1:
                            if time.time() < (start_time + 180): # timeout in seconds from start time
                                if self.tap.Coms_port.inWaiting():
                                    read = self.tap.Coms_port.read()
                                    sys.stdout.write(read)
                                    output_characters += read
                                    response_string = ''.join(output_characters)
                                    if ">" in response_string:
                                        break
                            else:
                                print("Timeout - Did not find response: %s in Tap response: (%s) " % (response_string))
                                break
                    else:
                        print(self.tap.SendCmdReadMulti(input))


if __name__ == '__main__':

        #print('Note this script assumes that the ___.bos file is sitting in the same directory as this file.')
        if 'linux' in sys.platform:
            print('Sorry, this script only works on Windows systems.')
            sys.exit(1)
        com_port = raw_input("Please enter Your com port -example 'COM3': ")
        print("\nProvide path to the file directory not including the file name.")
        path = raw_input("Otherwise hit ENTER if in same directory:")
        file_name = raw_input("Please enter the filename including the extension: ")
        print("\nEnter 'user', 'diags', 'speaker', 'blob, 'anim' to load to proper location.")
        filetype = raw_input("Type:")
        print(filetype)
        sw_update_script = LpmStandaloneUpdater(com_port, file_name, path, filetype)
        sw_update_script.update()
