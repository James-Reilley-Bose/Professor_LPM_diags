#!/usr/bin/python

import re
import codecs
import struct


class Segment:
    """
    Represents contiguous binary data along with a starting address.

    Each method will return a new segment with the desired properties.
    """
    def __init__(self, address, data):
        self.address = address
        self.data = data

    @staticmethod
    def fromPack(fmt, *args):
        s = struct.pack(fmt, *args)
        return Segment(0, s)

    def crop(self, start, end):
        start = max(start, self.address)
        end = min(end, self.address+len(self.data))
        if start is not None:
            start -= self.address
        if end is not None:
            end -= self.address
        return Segment(start, self.data[start:end])

    def offset(self, amount):
        return Segment(self.address + amount, self.data)

    def moveto(self, base):
        return Segment(base, self.data)

    def length(self):
        return len(self.data)

    def disp(self):
        dataStr = self.data.encode('hex')
        if len(dataStr) > 60:
            dataStr = dataStr[0:47] + ' ... ' + dataStr[-8:-1]
        print (hex(self.address), '-', hex(self.address+self.length()), ":", dataStr)


class SparseBuffer:
    """
    Represents a buffer containing non-contiguous binary data.

    Each method will return a NEW buffer with the desired properties.
    """

    def __init__(self):
        self.segments = []

    def mapsegments(self, f):
        """
        Maps function f to all segments, returns the modified SparseBuffer
        f takes a segment, and returns a modified segment.
        """
        sb = SparseBuffer()
        for segment in self.segments:
            modified_segment = f(segment)
            sb.addSegment(modified_segment)
        return sb

    def crop(self, start, end):
        """
        Returns only the contents between [start, end].
        """
        b = SparseBuffer()
        b.segments = [s.crop(start, end) for s in self.segments if s.crop(start, end).length() > 0]
        return b

    def __getitem__(self, s):
        """
        Allows array subscripting, performs same as crop().
        """
        return self.crop(s.start, s.stop)

    def offset(self, amount):
        """
        Offsets the start address of each segment by the given amount.
        """
        b = SparseBuffer()
        b.segments = [s.offset(amount) for s in self.segments]
        return b

    def moveto(self, base):
        """
        Offsets all segments so that the first one starts at the given address.
        """
        return self.offset(base - self.start())

    def addSegment(self, seg):
        """
        Adds a segment to this SparseBuffer.

        Note that this operation does NOT return a new SparseBuffer, but
        modifies its arguments.
        """
        before = filter(lambda s: s.address < seg.address, self.segments)
        after = filter(lambda s: s.address >= seg.address, self.segments)

        if len(before) > 0:
            lb = before[-1].address + before[-1].length()
            if lb > seg.address:
                raise Exception('Segment overlaps: {}, {}'.format(lb, seg.address))
            elif lb == seg.address:
                seg = Segment(before[-1].address, before[-1].data + seg.data)
                before.pop(-1)

        if len(after) > 0:
            fa = after[0].address
            if fa < seg.address + seg.length():
                raise Exception('Segment overlaps: {}, {}'.format(fa, seg.address + seg.length()))
            elif fa == seg.address + seg.length():
                seg = Segment(seg.address, seg.data + after[0].data)
                after.pop(0)

        self.segments = before + [seg] + after

    def merge(self, *others):
        """
        Returns all of its arguments merged into one SparseBuffer
        """
        n = SparseBuffer()
        n.segments = self.segments
        for b in others:
            for s in b.segments:
                n.addSegment(s)
        return n

    def __add__(self, other):
        """
        Convenient syntax for merge().
        """
        return self.merge(other)

    def __iadd__(self, other):
        """
        Convenient syntax for foo = foo.merge(...).
        """
        return self + other

    def fill(self, fillWith=0xff, start=0, end=None):
        """
        Returns a SparseBuffer with gaps filled with the specified value.
        """
        b = SparseBuffer()
        if end is None:
            end = self.end()
        term = Segment(end, '')
        lastAddr = start
        for s in self.segments+[term]:
            if s.address - lastAddr > 0:
                b.addSegment(Segment(lastAddr, chr(fillWith)*(s.address-lastAddr)))
            b.addSegment(s)
            lastAddr = b.end()
        assert(len(b.segments) == 1)
        return b

    def writebin(self, filename, fillWith=0xff):
        """
        Writes a SparseBuffer to a binary file.

        The buffer will have all gaps filled first.
        """
        lastAddr = 0
        with open(filename, 'wb') as f:
            for seg in self.segments:
                f.write(chr(fillWith) * (seg.address - lastAddr))
                f.write(seg.data)
                lastAddr = seg.address + seg.length()

    def writehex(self, filename, perLine=0x20):
        """
        Writes a SparseBuffer to an Intel Hex file.

        All gaps will be preserved.
        """
        segAddr = 0
        with open(filename, 'w') as f:
            f.write(ihexline(0, 4, 0))
            for s in self.segments:
                currAddr = s.address
                d = s.data
                off = 0
                while off < len(d):
                    if (currAddr & 0xffff0000) != segAddr:
                        segAddr = currAddr & 0xffff0000
                        f.write(ihexline(0, 4, segAddr >> 16))
                    l = min(len(d), perLine)
                    f.write(ihexline(currAddr & 0xffff, 0, d[off:(off+l)]))
                    off += l
                    currAddr += l
            f.write(ihexline(0, 1, ''))

    def append(self, seg):
        """
        Returns a SparseBuffer with the argument appended.
        """
        b = SparseBuffer()
        b.segments = self.segments
        b.addSegment(seg.moveto(b.end()))
        return b

    def start(self):
        """
        Returns the first address.
        """
        if not self.segments:
            return 0
        return min(s.address for s in self.segments)

    def end(self):
        """
        Returns one past the last address.
        """
        if not self.segments:
            return 0
        return max(s.address + s.length() for s in self.segments)

    def __len__(self):
        """
        Calls length()
        """
        return self.length()

    def length(self):
        """
        Returns the length, including any gaps.
        """
        return self.end() - self.start()

    def disp(self):
        map(Segment.disp, self.segments)

    def checksum32(self, fillWith=0xff, fmt='>', asSegment=True):
        """
        Returns a Segment with the 32-bit sum of bytes of the buffer.

        The SparseBuffer will have any gaps filled first.
        """
        filled = self.fill(fillWith, self.start(), self.end())
        s = sum(ord(c) for c in filled.segments[0].data) & 0xffffffff
        if asSegment:
            return Segment(0, struct.pack(fmt+'L', s))
        else:
            return s



def readhex(filename):
    """
    Reads an Intel Hex file and returns it in a SparseBuffer.

    Addresses are preserved, and the returned buffer may contain multiple
    Segments if the data is not contiguous.
    """
    f = open(filename, 'r')
    p = re.compile(':(..)(....)(..)((..)*)(..)')
    b = SparseBuffer()

    segmentBase = 0

    lastSeg = Segment(0, '')

    for line in f:
        m = p.match(line)
        (length,) = struct.unpack('B', m.group(1).decode('hex', 'strict'))
        (address,) = struct.unpack('!H', m.group(2).decode('hex', 'strict'))
        (rectype,) = struct.unpack('B', m.group(3).decode('hex', 'strict'))
        data = m.group(4).decode('hex', 'strict')

        address += segmentBase

        if rectype == 0x00:
            if address == lastSeg.address + lastSeg.length():
                lastSeg.data += data
            else:
                if lastSeg.length() > 0:
                    b.addSegment(lastSeg)
                lastSeg = Segment(address, data)
        elif rectype == 0x04:
            (segBase,) = struct.unpack('!H', data)
            segmentBase = segBase * 2**16
        elif rectype == 0x01:
            break
        else:
            raise Exception('Unknown record type')

    if lastSeg.length() > 0:
        b.addSegment(lastSeg)

    f.close()

    return b


def readbin(filename):
    """
    Reads a binary file and returns it in a SparseBuffer.

    This buffer contains a single Segment and starts at address 0.
    """
    with open(filename, 'rb') as f:
        b = SparseBuffer()
        b.addSegment(Segment(0, f.read()))
        return b


def readldr(filename):
    """
    Reads an ADI LDR file and returns it in a SparseBuffer.

    Each line in the LDR file must be either an 8-bit hex value (0xXX) or
    blank, any other content will return an error.  The returned buffer starts
    at address 0.

    This function ONLY supports the 8-bit "Include" format.
    """
    p = re.compile('0x(..),?')
    d = ''
    with open(filename, 'rb') as f:
        b = SparseBuffer()
        for line in f:
            m = p.match(line)
            if m is not None:
                d += m.group(1).decode('hex', 'strict')
            elif re.search('\S', line):
                raise Exception('Unexpected data', line)

    b = SparseBuffer()
    b.addSegment(Segment(0, d))
    return b


def ihexline(addr, rectype, data, fmt='>H'):
    """
    Returns an Intel Hex formatted line with the specified parameters.
    """
    if isinstance(data, (int, long)):
        data = struct.pack(fmt, data)
    length = len(data)
    cs = (length + rectype + (addr & 0xff) + ((addr >> 8) & 0xff) + sum(ord(c) for c in data))
    cs = (-cs) & 0xff
    return ':{:02X}{:04X}{:02X}{}{:02X}\n'.format(length, addr, rectype, data.encode('hex').upper(), cs)
