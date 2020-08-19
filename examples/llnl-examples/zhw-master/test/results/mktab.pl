#!/usr/bin/env perl

sub usage {
print <<USAGE;
Usage: mktab.pl <input file A>
USAGE
exit(1);
}

usage() if $#ARGV < 0 or $#ARGV > 1 or $ARGV[0] =~ /^-/;

# open input file A
open($fhi_a, "<", $ARGV[0]) or die(" -- error: could not open input $ARGV[0]: $!");

sub scan {
	my ($fhi, $tref) = @_;
	my $dims; # dimensions
	my $rate; # rate (bits)
	my $blocks; # blocks (n-dimensional, 4^n in size)
	while (<$fhi>) {
		# print $_;
		if (/dims: (\d+)/) {
			$dims = $1;
			next;
		}
		if (/rate: (\d+), blocks: (\d+)/) {
			$rate = $1;
			$blocks = $2;
			next;
		}
		if (/zfphw compression cycles: (\d+)/) {
			$tref->{$dims}{$blocks}{$rate}{HW} = $1;
			next;
		}
		if (/libzfp compression cycles: (\d+)/) {
			$tref->{$dims}{$blocks}{$rate}{SW} = $1;
			next;
		}
	}
}

my %tab;

scan($fhi_a, \%{$tab{_A}});
close($fhi_a);

print "\n\"ZFP HW cycles - $ARGV[0]\"\n";

my $tref = \%{$tab{_A}};
foreach my $_d (sort {$a <=> $b} keys %{$tref}) { # dims
	print "\n\"Dimensions: $_d\"\n";
	print "\"Blocks / Rate\"";
	my $ref = \%{$tref->{$_d}};
	foreach my $_x (sort {$a <=> $b} keys %{$ref->{(sort {$a <=> $b} keys %{$ref})[0]}}) { # rate
		print ", rate $_x";
	}
	print "\n";
	foreach my $_y (sort {$a <=> $b} keys %{$ref}) { # block
		print $_y;
		foreach my $_x (sort {$a <=> $b} keys %{$ref->{$_y}}) { # rate
			$tmp = $ref->{$_y}{$_x}{HW};
			print ", ", $tmp;
		}
		print "\n";
	}
}

print "\n\"ZFP SW cycles - $ARGV[0]\"\n";

my $tref = \%{$tab{_A}};
foreach my $_d (sort {$a <=> $b} keys %{$tref}) { # dims
	print "\n\"Dimensions: $_d\"\n";
	print "\"Blocks / Rate\"";
	my $ref = \%{$tref->{$_d}};
	foreach my $_x (sort {$a <=> $b} keys %{$ref->{(sort {$a <=> $b} keys %{$ref})[0]}}) { # rate
		print ", rate $_x";
	}
	print "\n";
	foreach my $_y (sort {$a <=> $b} keys %{$ref}) { # block
		print $_y;
		foreach my $_x (sort {$a <=> $b} keys %{$ref->{$_y}}) { # rate
			$tmp = $ref->{$_y}{$_x}{SW};
			print ", ", $tmp;
		}
		print "\n";
	}
}
