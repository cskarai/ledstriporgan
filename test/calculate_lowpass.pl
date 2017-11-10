#!/usr/bin/perl

use strict;
use warnings;

use Data::Dumper;
use Math::Trig ':pi';

my $step = 10 ** (1/50);

my $freq = 20;

my $cumm = 0;

my $svg = '<svg height="130" width="210">
  <line x1="35" y1="5" x2="35" y2="110" style="stroke:rgb(0,0,0);stroke-width:2" />
  <line x1="30" y1="105" x2="200" y2="105" style="stroke:rgb(0,0,0);stroke-width:2" />

  <line x1="30" y1="6" x2="35" y2="6" style="stroke:rgb(0,0,0);stroke-width:2" />

  <line x1="36" y1="6" x2="200" y2="6" style="stroke:rgb(224,224,224);stroke-width:2" />
  <line x1="85" y1="5" x2="85" y2="104" style="stroke:rgb(224,224,224);stroke-width:2" />
  <line x1="85" y1="105" x2="85" y2="110" style="stroke:rgb(0,0,0);stroke-width:2" />
  <line x1="135" y1="5" x2="135" y2="104" style="stroke:rgb(224,224,224);stroke-width:2" />
  <line x1="135" y1="105" x2="135" y2="110" style="stroke:rgb(0,0,0);stroke-width:2" />
  <line x1="185" y1="5" x2="185" y2="104" style="stroke:rgb(224,224,224);stroke-width:2" />
  <line x1="185" y1="105" x2="185" y2="110" style="stroke:rgb(0,0,0);stroke-width:2" />
  <line x1="36" y1="35" x2="200" y2="35" style="stroke:rgb(224,224,224);stroke-width:1" />

  <path d="###PATH###" style="stroke:rgb(0,0,255);stroke-width:2;fill:none;stroke-linejoin:round" />
  
  <text x="25" y="9" font-family="Arial" text-anchor="end" font-size="8">100%</text>
  <text x="25" y="108" font-family="Arial" text-anchor="end" font-size="8">0%</text>
  <text x="35" y="123" font-family="Arial" text-anchor="middle" font-size="8">20 Hz</text>
  <text x="85" y="123" font-family="Arial" text-anchor="middle" font-size="8">200 Hz</text>
  <text x="135" y="123" font-family="Arial" text-anchor="middle" font-size="8">2000 Hz</text>
  <text x="185" y="123" font-family="Arial" text-anchor="middle" font-size="8">20000 Hz</text>
  Sorry, your browser does not support inline SVG.
</svg>';


my @results;

print "Frequency,Ratio\n";

while( $freq < 22050 ) {
  my $ifreq = int($freq + 0.5);
  
  my @samples = generate_44100_audio_signals($ifreq);
  my $avg = compute_avg(\@samples);
  
  my $energy = compute_energy(\@samples, $avg);
  
  my @low_pass = compute_lowpass(\@samples);
  my $lavg = compute_avg(\@low_pass);
  
  my $lenergy = compute_energy(\@low_pass, $avg);
  
  my $ratio = int(100 * $lenergy / $energy + 0.5 );
  push @results, $ratio;
  
  print sprintf(" %5d Hz, %2d%%\n", $ifreq, $ratio);
  
  $freq = $freq * $step;
}

print "\n\n";
my $path = create_svg(\@results);
$svg =~ s/###PATH###/$path/;
print "$svg\n";

sub generate_44100_audio_signals
{
  my ($frq) = @_;
  my $period = 1 / $frq;
  my $step = 1 / 44100;

  my @arr;
  for( my $t=0; $t < 1; $t += $step ) {
	my $sin = int(2048 * sin( $t * pi2 / $period ) + 2048 );
	
	push @arr, $sin;
  }
  
  return @arr;
}

sub compute_avg
{
  my ($samples) = @_;
  
  my $avg = 0;
  $avg += $_ for (@$samples);
  
  $avg = int( $avg /@$samples + 0.5);
  return $avg;
}

sub compute_energy
{
  my ($samples, $avg) = @_;
  
  my $en = 0;
  for my $sample (@$samples)
  {
    my $sq = $sample - $avg;
	$en += $sq * $sq;
  }

  $en = sqrt( $en / @$samples );
  return int( $en + 0.5);
}

sub compute_lowpass
{
  return compute_lowpass_avg(@_);
}

sub compute_lowpass_sum
{
  my ($samples) = @_;
  my @split = @$samples;
  
  my @arr;
  
  while(@split) {
    my @chunk = splice @split, 0, 128;
	
	for my $smpl (@chunk) {
	  $cumm += 0.016 * ($smpl - 2048 - $cumm);
	}

	my @frm = (int($cumm + 2048.5), ) x 128;

	push @arr, @frm;
  }
  
  
  return @arr;
}

sub compute_lowpass_avg
{
  my ($samples) = @_;
  my @split = @$samples;
  
  my @arr;

  my $prev = [];
  while(@split) {
    my @chunk = splice @split, 0, 128;

    my $avg = compute_avg([@$prev, @chunk]);
    my @frm = ($avg) x 128;
	
	splice @chunk, 0, 86;
	$prev = \@chunk;
	
    push @arr, @frm;
  }
  
  return @arr;
}

sub create_svg
{
  my ($results) = @_;
  
  my $first = 1;
  my $ndx = 35;
  
  my @str;
  
  for my $res (@$results) {
    if( $first ) {
	  push @str, 'M';
	} else {
	  push @str, 'L';
	}
    $first = 0;
	
	push @str, sprintf("%d %d ", $ndx++, 100-$res + 5);
  }
  
  return join('', @str);
}
