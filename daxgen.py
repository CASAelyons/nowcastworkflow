#!/usr/bin/env python

import sys
import os
import pwd
import time
from Pegasus.DAX3 import *
from datetime import datetime
from argparse import ArgumentParser

class CASAWorkflow(object):
    def __init__(self, outdir, forecast_file):
        self.outdir = outdir
        self.forecast_file = forecast_file

    def generate_dax(self):
        "Generate a workflow"
        ts = datetime.utcnow().strftime('%Y%m%dT%H%M%SZ')
        dax = ADAG("casa_wf-%s" % ts)
        dax.metadata("name", "CASA")
        #USER = pwd.getpwuid(os.getuid())[0]
        #dax.metadata("creator", "%s@%s" % (USER, os.uname()[1]))
        #dax.metadata("created", time.ctime())

        # unzip files if needed
        ##nowcast_inputs = []
        #last_time = "0"
        ##for f in self.radar_files:
          ##  f = f.split("/")[-1]
           ## if f.endswith(".gz"):
             ##   nowcast_input = f[:-3]
               ## radar_inputs.append(nowcast_input)

               ## unzip = Job("gunzip")
               ## unzip.addArguments(f)
               ## unzip.uses(f, link=Link.INPUT)
               ## unzip.uses(nowcast_input, link=Link.OUTPUT, transfer=False, register=False)
               ## dax.addJob(unzip)
            ##else:
        ##nowcast_inputs.append(self.forecast)
            #string_start = f.find("-")
            #string_end = f.find(".", string_start)
            #file_time = f[string_start+1:string_end]
            #if file_time > last_time:
            #    last_time = file_time
        
        string_start = self.forecast_file[-1].find("-")
        string_end = self.forecast_file[-1].find(".", string_start)
        last_time = self.forecast_file[-1][string_start+1:string_end]

        #run merged reflectivity threshold
        mrtconfigfile = File("mrt_config.txt")
        my_forecast_file = File(self.forecast_file[-1])
        mrt_job = Job("mrtV2")
        mrt_job.addArguments("-c", mrtconfigfile)
        mrt_job.addArguments(" ".join(self.forecast_file[-1]))
        mrt_job.uses(mrtconfigfile, link=Link.INPUT)
        mrt_job.uses(self.forecast_file[-1], link=Link.INPUT)
        #ref_job.uses(max_reflectivity, link=Link.OUTPUT, transfer=True, register=False)
        dax.addJob(mrt_job)

        # generate image from max reflectivity
        colorscale = File("nexrad_ref.png")
        post_ref_job = Job("merged_netcdf2png")
        forecast_image = File(my_forecast_file.name[:-4]+".png")
        post_ref_job.addArguments("-c", colorscale, "-q 235 -z 0,75", "-o", forecast_image, my_forecast_file)
        post_ref_job.uses(my_forecast_file, link=Link.INPUT)
        post_ref_job.uses(colorscale, link=Link.INPUT)
        post_ref_job.uses(forecast_image, link=Link.OUTPUT, transfer=True, register=False)
        dax.addJob(post_ref_job)

        # Write the DAX file
        daxfile = os.path.join(self.outdir, dax.name+".dax")
        dax.writeXMLFile(daxfile)
        print daxfile

    def generate_workflow(self):
        # Generate dax
        self.generate_dax()

if __name__ == '__main__':
    parser = ArgumentParser(description="CASA Workflow")
    parser.add_argument("-f", "--files", metavar="INPUT_FILE", type=str, nargs="+", help="Forecast File", required=True)
    parser.add_argument("-o", "--outdir", metavar="OUTPUT_LOCATION", type=str, help="DAX Directory", required=True)

    args = parser.parse_args()
    outdir = os.path.abspath(args.outdir)
    
    if not os.path.isdir(args.outdir):
        os.makedirs(outdir)

    workflow = CASAWorkflow(outdir, args.files)
    workflow.generate_workflow()
