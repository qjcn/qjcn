#!/usr/bin/perl

use strict;
use FindBin qw($Bin);
use lib "$Bin";

my $fin = "JCNToolGuide.html";
my $fout = "guide.h";

my %opts;

use Getopt::Std;

# Perl trim function to remove whitespace from the start and end of the string
sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}

# Left trim function to remove leading whitespace
sub ltrim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	return $string;
}

# Right trim function to remove trailing whitespace
sub rtrim($)
{
	my $string = shift;
	$string =~ s/\s+$//;
	return $string;
}

getopts('i:o:',\%opts);
if ( $opts{i} )
{
    $fin = $opts{i};
}

if ( $opts{o} )
{
    $fout = $opts{o};
}

printf "input file: $fin\n";
printf "output file: $fout\n";


if  ( open(FILEOUT,">$fout") )
{
	chomp;
	s/^\s+//; # remove leading 
	s/^\s+//; # remove trailing
}
else
{
	printf("Can't open $fout\n");
	exit;
}

print FILEOUT "#ifndef help_h\n";
print FILEOUT "#define help_h\n";

print FILEOUT "static QString help_html =\"";
	
if  ( open(FILEIN,"<$fin") )
{
	while(<FILEIN>)
	{
#		chomp;
		# convert " to \"
		s/\"/\\"/g;	
#		s/\n/\ /g;

		# use c++ continuation char\
		s/\n/\\\n /g;
		
#		s/\</<\\/g;
		
		# add \ at the end of line
#		s/\n/\n\\"/;

		print FILEOUT $_;
	}
}
else
{
	printf("Can't open $fin\n");
	exit;
}

print FILEOUT "\";\n";
print FILEOUT "#endif\n";

