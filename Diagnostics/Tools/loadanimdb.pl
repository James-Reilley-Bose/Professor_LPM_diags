#!/usr/bin/perl
if (@ARGV < 2) {
    print "Usage: loadanimdb.pl LightbarAnimBinFile lpmserialdev\n";
    exit 0;
}

my $BLOCK_SIZE=128;
my $binbuffer;
my $hexbuffer;

my ($animdbbin, $serialdev) = @ARGV;
open(AnimDBFD, "< $animdbbin") || die "Can't open $animdbbin, $!\n";
open(SFD, "> $serialdev") || die "Can't open $serialdev, $!\n";
#Read animations DB file in binary mode
binmode(AnimDBFD);
# Start update
print SFD "lb ladbs\r\n";
print "lb ladbs\r\n";

#print "\n\n\nStart load successfull\n\n\n";

sleep(1);
# Write animation DB data to SPI flash
while (read(AnimDBFD,$binbuffer,$BLOCK_SIZE)) {
    #convert binary to hex
    $n = length($binbuffer);
    $s = 2*$n;
    $hexbuffer = unpack("H$s", $binbuffer);

   # print ("\n\n\nHexBuffer:$hexbuffer, count:$count\n\n\n");

    print SFD "lb ladb,$hexbuffer\r";
    print "lb ladb,$hexbuffer\r";
    select(undef,undef,undef,0.100);
    $count++;
}
# End AnimDB load
sleep(1);
print SFD "lb ladbe\r\n";
print "lb ladbe\r\n";

#print "\n\n\nEnd load successfull\n\n\n";

close(SFD);
close(AnimDBFD);
