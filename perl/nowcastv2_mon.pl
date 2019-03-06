#!/usr/bin/perl -w

##################################################################################################
#####WRITTEN BY ERIC LYONS 3/2018 for CASA, UNIVERSITY OF MASSACHUSETTS##########################
##################################################################################################
#  TESTED FUNCTIONALITY:         
#  Monitors nowcast directory
#  pqinserts nowcast files
#  plots nowcast files
#  pqinserts nowcast images
# 
#  #                                                                                                  #
##################################################################################################

use POSIX qw(setsid);
use File::Copy;
use File::Monitor;
use File::chdir;
use threads;
use threads::shared;

our $input_data_dir;

##Parse Command Line
&command_line_parse;

&daemonize;

##Realtime Mode -- Gets MCC stream
my $file_mon = new threads \&file_monitor;

sleep 900000000;

sub file_monitor {
    
    my $dir_monitor = File::Monitor->new();
        
    $dir_monitor->watch( {
	name        => "$input_data_dir",
	recurse     => 1,
        callback    => \&new_files,
    } );
    
    $dir_monitor->scan;
    
    for ($i=0; $i < 9000000000; $i++) {
	my @changes = $dir_monitor->scan;   
	sleep 10;
    }
    
    sub new_files 
    {
	my ($name, $event, $change) = @_;
	my @tmp = ();
	
	@new_netcdf_files = $change->files_created;
	my @dels = $change->files_deleted;
	print "Added: ".join("\nAdded: ", @new_netcdf_files)."\n" if @new_netcdf_files;
	foreach $file (@new_netcdf_files) {
	    sleep 1;
	    my $pathstr;
            my $filename;
            ($pathstr, $filename) = $file =~ m|^(.*[/\\])([^/\\]+?)$|;
	    my $filetype = "MERGE_DARTS";
	    if (index($filename, $filetype) != -1) {
		my $hmstr = substr($filename, -7, 4);
		my $ymdstr = substr($filename, -15, 8);
		system("NowcastToWDSS2 $file /data/images");
		print "pqinsert -f EXP -p nowcast_" . $ymdstr . "-" . $hmstr . "00.netcdf " . $file . "\n";
	    }
	    #system("merge_qpe_netcdf2png -c /home/ldm/netcdf2png/colorscales/standard_qpe.png -z 0,95.25 $file -o /data/images/multi/qpe1hr_multi_$ymdstr-$hmsstr.png");
	    #system("pqinsert -f EXP -p qpe1hr_multi_$ymdstr-$hmsstr.png /data/images/multi/qpe1hr_multi_$ymdstr-$hmsstr.png");
	}
    }
}

sub daemonize {
    chdir '/'                 or die "Can't chdir to /: $!";
    open STDIN, '/dev/null'   or die "Can't read /dev/null: $!";
    open STDOUT, '>>/dev/null' or die "Can't write to /dev/null: $!";
    open STDERR, '>>/dev/null' or die "Can't write to /dev/null: $!";
    defined(my $pid = fork)   or die "Can't fork: $!";
    exit if $pid;
    setsid                    or die "Can't start a new session: $!";
    umask 0;
}

sub command_line_parse {
    if ($#ARGV < 0) { 
	print "Usage:  dir_mon.pl netcdf_dir\n";
   	exit; 
    }
    $input_data_dir = $ARGV[0];
    
    my @rdd = split(/ /, $input_data_dir);
    foreach $w (@rdd) {
	print "Will recursively monitor $w for incoming netcdf files\n";
    }
    
}
