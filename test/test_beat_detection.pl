#!/usr/bin/perl

use strict;
use warnings;

use Data::Dumper;
use Getopt::Long;

use List::Util qw( min max );

my $type = 3;

GetOptions ("type=i" => \$type, ) or die("Hibás paraméter!\n");

my $infile = shift @ARGV;
die "Hiányzik a fájlnév!" if ! $infile;

my $outfile = shift @ARGV;
$outfile = './output.wav' if ! $outfile;

open( FH, "<", $infile ) or die "Can't open file to read: $!";
binmode FH;

open( WH, ">", $outfile ) or die "Can't open file to read: $!";
binmode WH;

my $hdr;
sysread(FH , $hdr , 44);
my $samples = unpack("V", substr($hdr, 40));

print WH $hdr;

my $rc = 0;

my @bass = (0,) x 10;
my @history = (0,) x 20;

while( $samples )
{
  my $count = $samples;
  $count = 512 if( $count > 512 );
  
  my $outbuf;
  my $buffer;
  sysread(FH , $buffer , $count);

  my $avg = 0;
  for( my $i=0; $i < $count; $i += 4 )
  {
    my $left = unpack( "s", substr($buffer, $i, 2));
    my $right = unpack( "s", substr($buffer, $i+2, 2));
  
    my $mid = int(($left + $right) / 2);
    
    $avg += $mid;
  }
  $avg /= ($count / 4);
  push @bass, $avg;
  shift @bass;
  
  my $bassStrength = calc_diff( @bass );
  push @history, $bassStrength;
  shift @history;
  
  my $minstrength = min(@history);
  
  my $diff = $bassStrength - $minstrength;
  
  for( my $i=0; $i < $count; $i += 4 )
  {
    my $leftIn = unpack( "s", substr($buffer, $i, 2));
    my $rightIn = unpack( "s", substr($buffer, $i+2, 2));
    
    my $mid = int(($leftIn + $rightIn) / 2);
    
    my $left = 0;
    my $right = 0;
    
    if($type == 0)
    {
      $left = $mid;
    } elsif($type == 1)
    {
      $left = $avg;
    } elsif($type == 2)
    {
      $left = $avg;
      $right = $bassStrength;
    } elsif($type == 3)
    {
      $left = $avg;
      $right = $diff;
    }
    
    $outbuf .= pack("s", $left);
    $outbuf .= pack("s", $right);
    
  }
  
  print WH $outbuf;
  
  $samples -= $count;
}

close FH;
close WH;

sub calc_diff
{
  my @arr = @_;
 
  my $energy = 0;
 
  for(my $q=1; $q < @arr; $q++) {
    my $diff = $arr[$q] - $arr[$q-1];
    $energy += ($diff * $diff);
  }
  $energy /= (@bass - 1);
  
  return sqrt($energy);
}
