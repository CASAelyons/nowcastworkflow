#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdarg.h>
#include "netcdf.h"

void handle_error(int errid){
  if(errid == NC_NOERR) return;
  printf("%s\n", nc_strerror(errid));
  return;
}

#define PR_Units       "dBZ"
#define NDIMS 3
#define NSTEPS 31
#define NLAT 201
#define NLON 201

size_t start[NDIMS], count[NDIMS];
float Refl_In[1][NLAT][NLON];

//char *timestamp;
int status;
int ncid_out, ncid;
int Steps_id, Lat_id, Lon_id, Lat_Out_id, Lon_Out_id, Refl_id;
int Nowcast_id, Nowcast_dimids[2], Nowcast_Out_id;
size_t t_len;
int i;
size_t num_steps, num_lats, num_lons;

int NowcastToWDSS2(char i_filename[], char o_directory[])
{
  size_t filename_len;
  filename_len = strlen(i_filename);
  char starttime[16];
  char yyyy[4];
  char mon[3];
  char dd[3];
  char hh[3];
  char mm[3];
  int ymd_start = (int)filename_len - 15;
  memcpy(yyyy, &i_filename[ymd_start], 4);
  yyyy[4] = '\0';
  memcpy(mon, &i_filename[ymd_start+4], 2);
  mon[2] = '\0';
  memcpy(dd, &i_filename[ymd_start+6], 2);
  dd[2] = '\0';
  memcpy(hh, &i_filename[ymd_start+8], 2);
  hh[2] = '\0';
  memcpy(mm, &i_filename[ymd_start+10], 2);
  mm[2] = '\0';
  starttime[15] = '\0';
  sprintf(starttime, "%s%s%s-%s%s00", yyyy, mon, dd, hh, mm);
  //printf("%s\n", starttime);

  status = nc_open(i_filename, NC_NOWRITE, &ncid);
  handle_error(status);

  status = nc_inq_dimid(ncid, "Steps", &Steps_id);
  handle_error(status);
  
  status = nc_inq_dimlen(ncid, Steps_id, &num_steps);
  handle_error(status);
  
  status = nc_inq_dimid(ncid, "Lat", &Lat_id);
  handle_error(status);
  
  status = nc_inq_dimlen(ncid, Lat_id, &num_lats);
  handle_error(status);

  status = nc_inq_dimid(ncid, "Lon", &Lon_id);
  handle_error(status);

  status = nc_inq_dimlen(ncid, Lon_id, &num_lons);
  handle_error(status);
  
  status = nc_inq_attlen(ncid, NC_GLOBAL, "Time", &t_len);
  handle_error(status);

  //timestamp = (char *)malloc(t_len + 1);
  //status = nc_get_att_text(ncid, NC_GLOBAL, "Time", timestamp);
  
  count[0] = 1;
  count[1] = NLAT;
  count[2] = NLON;

  
  for (i=0;i<NSTEPS;i++) {
    start[0] = i;
    start[1] = 0;
    start[2] = 0;

    float Refl_In[1][NLAT][NLON];

    status = nc_inq_varid(ncid, "PredictedReflectivity", &Refl_id);
    handle_error(status);

    status = nc_get_vara_float(ncid, Refl_id, start, count, &Refl_In[0][0][0]);
    handle_error(status);
    
    static char datatype[] = "LatLonGrid";
    static int fractionaltime[]= {0};
    Nowcast_dimids[0] = Lat_Out_id;
    Nowcast_dimids[1] = Lon_Out_id;
    
    char typename [32];
    sprintf(typename, "PredictedReflectivity_%dmin", i);

    char filename [256];
    sprintf(filename, "%s/%s_%s.nc", o_directory, typename, starttime);
    
    status = nc_create(filename, NC_CLOBBER, &ncid_out);
    handle_error(status);
 
    status = nc_def_dim(ncid_out, "Lat", num_lats, &Lat_Out_id);
    handle_error(status);

    status = nc_def_dim(ncid_out, "Lon", num_lons, &Lon_Out_id);
    handle_error(status);
    
    status = nc_def_var(ncid_out, typename, NC_FLOAT, 2, Nowcast_dimids, &Nowcast_id);
    handle_error(status);
 
    status = nc_put_att_text(ncid_out, Nowcast_id, "Units",strlen(PR_Units), PR_Units);
    handle_error(status);
      
    status = nc_put_att_text(ncid_out, NC_GLOBAL, "TypeName", strlen(typename), typename);
    handle_error(status);
    
    status = nc_put_att_text(ncid_out, NC_GLOBAL, "DataType", strlen(datatype), datatype);
    handle_error(status);
    
    status = nc_put_att_int(ncid_out, NC_GLOBAL, "FractionalTime", NC_INT, 1, fractionaltime);
    handle_error(status);
 
    status =  nc_copy_att (ncid, NC_GLOBAL, "Latitude", ncid_out, NC_GLOBAL);
    handle_error(status);
    
    status =  nc_copy_att (ncid, NC_GLOBAL, "Longitude", ncid_out, NC_GLOBAL);
    handle_error(status);
    
    status =  nc_copy_att (ncid, NC_GLOBAL, "Height", ncid_out, NC_GLOBAL);
    handle_error(status);
    
    status =  nc_copy_att (ncid, NC_GLOBAL, "Time", ncid_out, NC_GLOBAL);
    handle_error(status);
    
    status =  nc_copy_att (ncid, NC_GLOBAL, "LatGridSpacing", ncid_out, NC_GLOBAL);
    handle_error(status);
    
    status =  nc_copy_att (ncid, NC_GLOBAL, "LonGridSpacing", ncid_out, NC_GLOBAL);
    handle_error(status);
    
    status =  nc_copy_att (ncid, NC_GLOBAL, "MissingData", ncid_out, NC_GLOBAL);
    handle_error(status);

    status = nc_enddef(ncid_out);
    
    status = nc_put_var_float(ncid_out, Refl_id, &Refl_In[0][0][0]);
    handle_error(status);
    
    status = nc_close(ncid_out);
    handle_error(status);
  }
  
  return 0;
}
