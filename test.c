/*
 * test.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: caojinfeng
 */


#include "config.h"
#include "common/osdep.h"
#include "lsmash.h"
#include "cli.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "importer/importer.h"


int test(lsmash_root_t  *root,const void* pData, unsigned int size, unsigned int encodeType);
int main(int argc,char *argv[])
{
	lsmash_root_t 			 *root;
	lsmash_file_t            *file;
	lsmash_file_parameters_t *file_param = (lsmash_file_parameters_t *)malloc(sizeof(lsmash_file_parameters_t));
	lsmash_movie_parameters_t movie_param;
	lsmash_track_parameters_t track_param;
	lsmash_media_parameters_t media_param;
	uint32_t          sample_entry;
	   int                  lpcm;
	 lsmash_summary_t *summary;
	FILE *inflie = fopen( argv[1], "rb" );
	root = lsmash_create_root();
	if( !root )
		return -1;
	if( lsmash_open_file(argv[2], 0, file_param ) < 0 )
	{
		 printf("failed to open an output file.\n");
		 return -1;
	}
	file_param->major_brand   = 1836069938;
	file_param->brands = (uint32_t *)malloc(sizeof(uint32_t)*50);
	file_param->brands[0] = ISOM_BRAND_TYPE_MP42;
	file_param->brands[1] = ISOM_BRAND_TYPE_MP41;
	file_param->brands[2] = ISOM_BRAND_TYPE_ISOM;
	file_param->brands[3] = ISOM_BRAND_TYPE_AVC1;
    file_param->brand_count   = 4;
    file_param->minor_version = 0;
	file = lsmash_set_file( root, file_param );
	if( !file)
	{
		 printf("failed to add an output file into a ROOT.\n");
		 return -1;
	}

	lsmash_initialize_movie_parameters( &movie_param );
	lsmash_initialize_media_parameters( &media_param );
	lsmash_initialize_track_parameters( &track_param );
	 movie_param.timescale = 25;
	if( lsmash_set_movie_parameters( root, &movie_param ) )
	{
		 printf("failed to set movie parameters.\n" );
		 return -1;
	}
	track_param.mode = ISOM_TRACK_IN_MOVIE | ISOM_TRACK_IN_PREVIEW;
	if( lsmash_set_movie_parameters( root, &movie_param ) )
	{
		 printf("failed to set movie parameters.\n" );
		 return -1;
	}
	/* Set track parameters. */
	uint32_t track_ID = lsmash_create_track( root, ISOM_MEDIA_HANDLER_TYPE_VIDEO_TRACK );

	if( lsmash_set_track_parameters( root, track_ID, &track_param ) )
	{
		 printf("failed to set track parameters.\n" );
		 return -1;
	}
	 media_param.timescale          = 25;
	 summary = lsmash_create_summary( LSMASH_SUMMARY_TYPE_VIDEO );
	 sample_entry = lsmash_add_sample_entry( root, track_ID, summary );
     if( !sample_entry )
     {
    	 printf("failed to add sample description entry.\n" );
    	 		 return -1;
     }
	/* Set media parameters. */
	if( lsmash_set_media_parameters( root, track_ID, &media_param ) )
	{
		printf("failed to set media parameters.\n" );
		return -1;
	}
	lsmash_construct_timeline( root, track_ID ) ;
	while(1)
	{

		lsmash_sample_t  *sample = (lsmash_sample_t *)malloc(sizeof(lsmash_sample_t));
		if(fread(sample, sizeof(sample),  1,inflie) == 0 )
		{
			break;
		}
		fseek(inflie,sizeof(sample),SEEK_CUR);
		sample->data = (uint8_t *)malloc(sample->length*sizeof(uint8_t));
		if(fread(sample->data, sample->length*sizeof(uint8_t),1, inflie) == 0)
		{
			break;
		}
		fseek(inflie,sample->length,SEEK_CUR);
//		printf("\n#######################################\n");
//    	printf("sample->cts = %"PRIu64"\n",sample->cts);
//		printf("sample->dts = %"PRIu64"\n",sample->dts);
//		printf("sample->pos = %"PRIu64"\n",sample->pos);
//		printf("sample->length =%"PRIu32"\n",sample->length);
//		printf("sample->index = %"PRIu32"\n",sample->index);
		int ret =  lsmash_append_sample( root, track_ID, sample );
		if(ret )
		{
			printf("failed to append a sample.  ret = %d\n",ret);
			return -1;
		}

	}
	 fclose(inflie);
	return lsmash_finish_movie( root, NULL ) ;
}

int test(lsmash_root_t  *root,const void* pData, unsigned int size, unsigned int encodeType)
{
	lsmash_sample_t  *sample = (lsmash_sample_t *)malloc(sizeof(lsmash_sample_t));
	uint32_t          track_ID;
	sample->index = 0;
	sample->length = size;
	sample->data = (uint8_t *)pData;
	sample->dts = 720;
	sample->cts = 720;
	track_ID = lsmash_create_track( root, ISOM_MEDIA_HANDLER_TYPE_VIDEO_TRACK );
	int ret =  lsmash_append_sample( root, track_ID, sample );
	if(ret )
	{
		printf("failed to append a sample.  ret = %d\n",ret);
		return -1;
	}

	return 0;
}

