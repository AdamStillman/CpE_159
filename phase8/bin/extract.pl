#!/usr/bin/perl
# extract.pl
# W. Chang, copyfree 2015
# simple Perl script to extract/convert executable file to text
# input: executable filename from shell command
# output: text file of same name with .x in extension

$#ARGV == 0 || die "Usage: extract.pl (executable/file name)\n";

$in_file = $ARGV[0];
$out_file = "$in_file.x";

open IN, "< $in_file" || die "Can't read $in_file,";     # read buffer IN
open OUT, "> $out_file" || die "Can't write $out_file,"; # write buffer OUT

binmode IN;   # read in binary mode

$in_size = 0;
$bytes = "";
while (($n = read IN, $byte, 1) != 0) { # read a byte at a time till EOF
   $bytes .= $byte;                     # concatenate them up
   $in_size++;
}

$out_size = $in_size;     # keep header, or $in_size - 128; # skip header
print "$in_file is $in_size bytes, $out_file will contain $out_size pairs of hex digits.\n";

for $i (0 .. $in_size - 1) {
#   next if($i < 128); # skip header, 1st 128 bytes

   $byte = substr($bytes, $i, 1); # get a char in the string (byte stream)

   $str = sprintf(" 0x%x", ord($byte)); # convert output to hex format

   print OUT $str;

   print OUT "," unless($i == $in_size - 1); # separate numbers with , unless last

   if((($i + 1) % 8 == 0) || ($i == $in_size - 1)) {
      print OUT "\n"; # every 8 hex pairs or last number, add newline
   }
}

close OUT; # close output file handle

