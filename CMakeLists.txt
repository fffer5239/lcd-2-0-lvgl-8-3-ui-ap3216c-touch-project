cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

SET(CMAKE_C_FLAGS " -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -Dgcc -O3 -gdwarf-2 -g")


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME project)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/retarget.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/lckfb_log/lckfb_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/lckfb_log/lckfb_log_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/lcd/bsp_backlight.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/lcd/st7789.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/lcd/st7789_font.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/lcd/st7789_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/soft_i2c/soft_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/soft_i2c/soft_i2c_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/touch/ft6336_drv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/stm32f4xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/stm32f4xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/examples/porting/lv_port_disp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/examples/porting/lv_port_fs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/examples/porting/lv_port_indev.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_disp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_group.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_indev.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_indev_scroll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_obj.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_obj_class.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_obj_draw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_obj_pos.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_obj_scroll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_obj_style.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_obj_style_gen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_obj_tree.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_refr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/core/lv_theme.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/arm2d/lv_gpu_arm2d.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_draw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_draw_arc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_draw_img.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_draw_label.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_draw_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_draw_line.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_draw_mask.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_draw_rect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_draw_transform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_draw_triangle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_img_buf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_img_cache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/lv_img_decoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/pxp/lv_draw_pxp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/pxp/lv_draw_pxp_blend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/pxp/lv_gpu_nxp_pxp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/pxp/lv_gpu_nxp_pxp_osa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/vglite/lv_draw_vglite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/vglite/lv_draw_vglite_arc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/vglite/lv_draw_vglite_blend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/vglite/lv_draw_vglite_line.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/vglite/lv_draw_vglite_rect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/vglite/lv_vglite_buf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/nxp/vglite/lv_vglite_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/renesas/lv_gpu_d2_draw_label.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/renesas/lv_gpu_d2_ra6m3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_arc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_bg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_composite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_img.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_label.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_line.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_mask.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_polygon.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_rect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_stack_blur.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_texture_cache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sdl/lv_draw_sdl_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/stm32_dma2d/lv_gpu_stm32_dma2d.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_arc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_blend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_dither.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_gradient.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_img.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_letter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_line.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_polygon.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_rect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/sw/lv_draw_sw_transform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/draw/swm341_dma2d/lv_gpu_swm341_dma2d.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/layouts/flex/lv_flex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/layouts/grid/lv_grid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/bmp/lv_bmp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/ffmpeg/lv_ffmpeg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/freetype/lv_freetype.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/fsdrv/lv_fs_fatfs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/fsdrv/lv_fs_littlefs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/fsdrv/lv_fs_posix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/fsdrv/lv_fs_stdio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/fsdrv/lv_fs_win32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/gif/gifdec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/gif/lv_gif.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/png/lodepng.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/png/lv_png.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/qrcode/lv_qrcode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/qrcode/qrcodegen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/rlottie/lv_rlottie.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/sjpg/lv_sjpg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/sjpg/tjpgd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/libs/tiny_ttf/lv_tiny_ttf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/lv_extra.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/others/fragment/lv_fragment.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/others/fragment/lv_fragment_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/others/gridnav/lv_gridnav.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/others/ime/lv_ime_pinyin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/others/imgfont/lv_imgfont.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/others/monkey/lv_monkey.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/others/msg/lv_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/others/snapshot/lv_snapshot.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/themes/basic/lv_theme_basic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/themes/default/lv_theme_default.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/themes/mono/lv_theme_mono.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/animimg/lv_animimg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/calendar/lv_calendar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/calendar/lv_calendar_header_arrow.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/calendar/lv_calendar_header_dropdown.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/chart/lv_chart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/colorwheel/lv_colorwheel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/imgbtn/lv_imgbtn.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/keyboard/lv_keyboard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/led/lv_led.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/list/lv_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/menu/lv_menu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/meter/lv_meter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/msgbox/lv_msgbox.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/span/lv_span.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/spinbox/lv_spinbox.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/spinner/lv_spinner.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/tabview/lv_tabview.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/tileview/lv_tileview.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/extra/widgets/win/lv_win.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_dejavu_16_persian_hebrew.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_fmt_txt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_loader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_10.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_12.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_12_subpx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_14.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_18.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_20.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_22.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_24.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_26.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_28.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_28_compressed.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_30.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_34.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_36.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_38.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_40.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_42.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_44.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_46.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_48.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_montserrat_8.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_simsun_16_cjk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_unscii_16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/font/lv_font_unscii_8.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/hal/lv_hal_disp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/hal/lv_hal_indev.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/hal/lv_hal_tick.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_anim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_anim_timeline.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_area.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_async.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_bidi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_color.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_fs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_gc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_ll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_lru.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_math.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_printf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_style.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_style_gen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_templ.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_tlsf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_txt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_txt_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/misc/lv_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_arc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_bar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_btn.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_btnmatrix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_canvas.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_checkbox.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_dropdown.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_img.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_label.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_line.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_objx_templ.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_roller.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_slider.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_switch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_table.c
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/widgets/lv_textarea.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32f407xx.s

    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/ap3216c/ap3216c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/lcd-lvgl-test/ui_test.c

)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/lcd
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/lckfb_log
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/soft_i2c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/touch
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/ap3216c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/lcd-lvgl-test
    ${CMAKE_CURRENT_SOURCE_DIR}/LIB/LVGL-8.3/src/hal

)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    USE_HAL_DRIVER
    STM32F407xx
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

