#!/usr/bin/perl

use strict;
use FindBin qw($Bin);
use lib "$Bin";

my $fin = "jcn_strings.txt";
my $fout = "jcn_strings_data.h";

my %opts;

use Getopt::Std;

my %str_h;

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

# convert to upper case
sub toupper($)
{
	my $string = shift;
	$string = uc $_;
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

my $tstr;

print FILEOUT "#ifndef jcn_strings_data_h\n";
print FILEOUT "#define jcn_strings_data_h\n";

print FILEOUT "static const char * jcn_strings_data [] =\n";
print FILEOUT "{\n";
	
if  ( open(FILEIN,"<$fin") )
{
	while(<FILEIN>)
	{
		chomp;
		s/\n/\ /g;;  # subst new line for space
		s/\r/\ /g;;  # subst carriage return for space
		s/\\/\ /g;  # subst slash return for space
		
		$tstr = toupper($_);
		
		if ( exists $str_h{$tstr} )
		{
			next;
		}

		$str_h{$tstr} = "XXX";

		print FILEOUT "\"".$tstr."\",\n";
	}
}
else
{
	printf("Can't open $fin\n");
	exit;
}

print FILEOUT "\".\"\n";
print FILEOUT "};\n";

print FILEOUT "#endif\n";

