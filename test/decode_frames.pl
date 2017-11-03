#!/usr/bin/perl

use warnings;

use Data::Dumper;
use Getopt::Long;

my $infile = shift @ARGV;
$infile = "./data.bin" if ! $infile;

my $outprefix = shift @ARGV;
$outprefix = "./output_" if ! $outprefix;

open( FH, "<", $infile ) or die "Can't open file to read: $!";
binmode FH;

my $buf = 0;

my @frames;

my $lastFrame;

while(1)
{
  last if sysread(FH , $buf , 1) != 1;
  next if ord($buf) != 0xb1;
  
  last if sysread(FH , $buf , 1) != 1;
  next if ord($buf) != 0xda;

  last if sysread(FH , $buf , 1) != 1;
  next if ord($buf) != 0x0e;

  last if sysread(FH , $buf , 1) != 1;
  next if ord($buf) != 0xc5;

  last if sysread(FH , $buf , 2) != 2;
  my $avg = unpack("s", $buf);

  last if sysread(FH , $buf , 1) != 1;
  my $frame = unpack("C", $buf);

  if( defined $lastFrame ) {
    if( $frame != (($lastFrame + 1) & 0xFF ) ) {
      print "Frame mismatch: " . $frame . "/" . $lastFrame . "\n";
    }
  }
  
  $lastFrame = $frame;
  
  last if sysread(FH , $buf , 1) != 1;
  my $flags = unpack("C", $buf);

  last if sysread(FH , $buf , 2) != 2;
  my $bassStrength = unpack("s", $buf);

  last if sysread(FH , $buf , 64) != 64;
  my @data = ();
  
  for( my $c=0; $c < length($buf); $c += 2) {
    my $d = unpack("s", substr($buf, $c, 2));
    push @data, $d - 8192;
  }

  my %frame = (frame => $frame, avg => $avg, flags => $flags, 
               bassStrength => $bassStrength, data => \@data,);
  
  push @frames, \%frame;
}

close(FH);

my $wh = create_wav("1_music.wav");

for my $frame (@frames)
{
  for my $data (@{$frame->{data}}) {
    print $wh pack("s", $data * 2);
  }
}

close($wh);

$wh = create_wav("2_avg.wav");

for my $frame (@frames)
{
  for(my $i=0; $i < 32; $i++) {
    print $wh pack("s", ($frame->{avg} - 2048) * 16);
  }
}

close($wh);

$wh = create_wav("3_bass_strength.wav");

for my $frame (@frames)
{
  for(my $i=0; $i < 32; $i++) {
    print $wh pack("s", $frame->{bassStrength} * 4);
  }
}

$wh = create_wav("4_beat_state.wav");

for my $frame (@frames)
{
  my $state = $frame->{flags} & 7;
  
  my $val = 0;
  
  if( $state == 0 ) { # no beat
    $val = 0;
  } elsif( $state == 1 ) { # in beat
    $val = 8192;
  } elsif( $state == 2 ) { # beat fall
    $val = 4096;
  } elsif( $state == 3 ) { # beat disable
    $val = -4096;
  }
  
  for(my $i=0; $i < 32; $i++) {
    print $wh pack("s", $val);
  }
}

$wh = create_wav("5_beat.wav");

for my $frame (@frames)
{
  for(my $i=0; $i < 32; $i++) {
    if( $frame->{flags} & 128 ) {
      print $wh pack("s", 4096);
    } else {
      print $wh pack("s", 0);
    }
  }
}

close($wh);

sub create_wav {
  my ($name) = @_;
  
  open( my $wh, ">", $outprefix . $name ) or die "Can't open file to write: $!";
  binmode $wh;

  print $wh "RIFF";

  my $wavfilesize = @frames * 32 * 2;

  print $wh pack("V", $wavfilesize + 44 - 8);
  print $wh "WAVEfmt ";
  print $wh pack("V", 16); # chunk size
  print $wh pack("v", 1);  # PCM
  print $wh pack("v", 1); # number of channels
  print $wh pack("V", 11025);

  print $wh pack("V", 2); # chunk size
  print $wh pack("v", 2); # bytes for a sample
  print $wh pack("v", 8 * 2);
  print $wh 'data';
  print $wh pack("V", $wavfilesize);

  return $wh;
}
