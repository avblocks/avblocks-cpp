cmake_minimum_required(VERSION 3.16)

# meta project that includes all other projects
project(samples)

# Operating System
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

# add projects, each project should be in its own directory
if (OS STREQUAL "darwin")
    add_subdirectory(${OS}/dec_aac_adts_file)
    add_subdirectory(${OS}/dec_aac_adts_pull)

    add_subdirectory(${OS}/dec_avc_au)
    add_subdirectory(${OS}/dec_avc_file)
    
    add_subdirectory(${OS}/dec_g711_alaw_file)
    add_subdirectory(${OS}/dec_g711_ulaw_file)

    add_subdirectory(${OS}/dec_mp3_file)
    
    add_subdirectory(${OS}/demux_mp4_file)
    
    add_subdirectory(${OS}/enc_aac_adts_file)
    add_subdirectory(${OS}/enc_aac_adts_pull)
    add_subdirectory(${OS}/enc_aac_adts_push)
    
    add_subdirectory(${OS}/enc_avc_file)
    add_subdirectory(${OS}/enc_avc_pull)

    add_subdirectory(${OS}/enc_g711_alaw_file)
    add_subdirectory(${OS}/enc_g711_ulaw_file)

    add_subdirectory(${OS}/enc_mp3_file)
    add_subdirectory(${OS}/enc_mp3_pull)
    add_subdirectory(${OS}/enc_mp3_push)
    
    add_subdirectory(${OS}/enc_preset_file)

    add_subdirectory(${OS}/info_metadata_file)
    add_subdirectory(${OS}/info_stream_file)

    add_subdirectory(${OS}/mux_mp4_file)

    add_subdirectory(${OS}/re-encode)
    add_subdirectory(${OS}/slideshow)
endif()

if(OS STREQUAL "linux")
    add_subdirectory(${OS}/dec_aac_adts_file)
    add_subdirectory(${OS}/dec_aac_adts_pull)

    add_subdirectory(${OS}/dec_avc_au)
    add_subdirectory(${OS}/dec_avc_file)

    add_subdirectory(${OS}/dec_g711_alaw_file)
    add_subdirectory(${OS}/dec_g711_ulaw_file)

    add_subdirectory(${OS}/dec_mp3_file)

    add_subdirectory(${OS}/demux_mp4_file)

    add_subdirectory(${OS}/enc_aac_adts_file)
    add_subdirectory(${OS}/enc_aac_adts_pull)
    add_subdirectory(${OS}/enc_aac_adts_push)

    add_subdirectory(${OS}/enc_avc_file)
    add_subdirectory(${OS}/enc_avc_pull)

    add_subdirectory(${OS}/enc_g711_alaw_file)
    add_subdirectory(${OS}/enc_g711_ulaw_file)

    add_subdirectory(${OS}/enc_mp3_file)
    add_subdirectory(${OS}/enc_mp3_pull)
    add_subdirectory(${OS}/enc_mp3_push)

    add_subdirectory(${OS}/enc_preset_file)

    add_subdirectory(${OS}/info_metadata_file)
    add_subdirectory(${OS}/info_stream_file)

    add_subdirectory(${OS}/mux_mp4_file)

    add_subdirectory(${OS}/re-encode)
    add_subdirectory(${OS}/slideshow)
endif()

if(OS STREQUAL "windows")
    add_subdirectory(${OS}/dec_aac_adts_file)
    add_subdirectory(${OS}/dec_aac_adts_pull)
    
    add_subdirectory(${OS}/dec_avc_au)
    add_subdirectory(${OS}/dec_avc_file)

    add_subdirectory(${OS}/dec_g711_alaw_file)
    add_subdirectory(${OS}/dec_g711_ulaw_file)

    add_subdirectory(${OS}/dec_mp3_file)
 
    add_subdirectory(${OS}/demux_mp4_file)

    add_subdirectory(${OS}/enc_aac_adts_file)
    add_subdirectory(${OS}/enc_aac_adts_pull)
    add_subdirectory(${OS}/enc_aac_adts_push)
    
    add_subdirectory(${OS}/enc_avc_file)
    
    add_subdirectory(${OS}/enc_mp3_file)
    add_subdirectory(${OS}/enc_mp3_pull)
    add_subdirectory(${OS}/enc_mp3_push)
    
    add_subdirectory(${OS}/enc_preset_file)

    add_subdirectory(${OS}/info_metadata_file)
    add_subdirectory(${OS}/info_stream_file)

    add_subdirectory(${OS}/mux_mp4_file)

    add_subdirectory(${OS}/re-encode)
    add_subdirectory(${OS}/slideshow)
endif()
