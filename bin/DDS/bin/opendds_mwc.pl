#!/usr/bin/env perl
if (!defined($ENV{ACE_ROOT}) || !defined($ENV{MPC_ROOT})) {
  die("The enviroment needs to be setup.");
}
system('perl', "$ENV{ACE_ROOT}/bin/mwc.pl", "-type", "vs2022", @ARGV) == 0
  or die("Failed to run mwc: $!");
