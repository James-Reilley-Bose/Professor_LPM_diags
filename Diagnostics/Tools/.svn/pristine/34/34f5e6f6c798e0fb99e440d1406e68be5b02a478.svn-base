#!/usr/bin/perl -w
#
# This script will generate the Version.h file from a template.  It
# reads the template from stdin and writes the contents to stdout.
# It creates the Build Number field by using the svnversion utility,
# unless it is run in an Electric Commander job, in which case it
# gets it from the svnRev property.  But both of these can be
# overridden with the -revision option, which is needed on some
# projects like Shelby.  The field from which to extract the branch
# name needs to be adjusted for each project (for example, it's $6
# for this project as shown on the line defining $svnbranch below).

use File::Basename;
use Getopt::Long;
use Sys::Hostname;

my $status = GetOptions("revision=s", \$revision);
exit(1) unless ($status);

my %values;
my $basename = basename($0);
my $svnversion = defined($ENV{COMMANDER_JOBID}) ? `ectool getProperty /myJob/svnRev` : `svnversion`; chomp($svnversion);
my $svnbranch =  defined($ENV{COMMANDER_JOBID}) ? `ectool getProperty /myJob/svnURL | awk -F/ '{print \$NF}'` : `svn info | awk -F/ "/^URL:/ {print \$NF}"`; chomp($svnbranch);
my $buildtype =  defined($ENV{COMMANDER_JOBID}) ? `ectool getProperty /myJob/BuildType` : "Developer" ; chomp($buildtype);
$svnversion = $1 if($svnversion =~ m/\S+:(.*)/);
my ($sec, $min, $hour, $day, $mon, $year, @rest) = localtime; $year += 1900; $mon++;
$svnversion =~ s/\r//;  # Get rid of the pesky CR if we happen to run on Windows

# If the revision was specified on the command line, it overrides anything we got from the environment
$svnversion = $revision if (defined($revision));

$values{BUILDNUMBER} = $svnversion;
$values{BRANCHNAME}  = $svnbranch;
$values{BUILDTYPE}   = $buildtype;
$values{DEVELOPER}   = $ENV{USER} || $ENV{USERNAME} || $ENV{LOGNAME};
$values{MACHINE}     = lc((split(/\./, hostname))[0]);
$values{BUILDTIME}   = sprintf("%4d-%02d-%02dT%02d:%02d:%02d", $year, $mon, $day, $hour, $min, $sec);
$values{YEAR}        = $year;

print "// This file was generated with $basename at build time -- DO NOT EDIT BY HAND\n";
while(<STDIN>)
{
    next if (m|^//|);
    if (/<(\w+)>/)
    {
       $key = $1;
       s/<$key>/$values{$key}/ if defined($values{$key});
    }
    print;
}
exit(0);
