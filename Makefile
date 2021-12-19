EE_OBJS = am_map.o cosmitoFileIO.o d_items.o d_main.o d_net.o doomdef.o doomstat.o \
dstrings.o f_finale.o f_wipe.o g_game.o hu_lib.o hu_stuff.o i_main.o \
i_net.o i_sound.o i_system.o i_video.o info.o m_argv.o m_bbox.o \
m_cheat.o m_fixed.o m_menu.o m_misc.o m_random.o m_swap.o mixer_thread.o mixer.o mmus2mid.o p_ceilng.o \
p_doors.o p_enemy.o p_floor.o p_inter.o p_lights.o p_map.o p_maputl.o \
p_mobj.o p_plats.o p_pspr.o p_saveg.o p_setup.o p_sight.o p_spec.o \
p_switch.o p_telept.o p_tick.o p_user.o ps2doom.o r_bsp.o r_data.o r_draw.o \
r_main.o r_plane.o r_segs.o r_sky.o r_things.o s_sound.o sjpcm_rpc.o sounds.o \
st_lib.o st_stuff.o tables.o v_video.o w_wad.o w_mmap.o wi_stuff.o z_zone.o usbd.s usbhdfsd.s SJPCM.s \

EE_BIN = ps2doom.elf

EE_INCS = -I$(PS2SDK)/ports/include/SDL -I$(PS2SDK)/ports/include -I$(PS2DEV)/isjpcm/include/ -I$(PS2DEV)/isjpcm/
EE_LDFLAGS = -L$(PS2SDK)/ports/lib -L$(PS2DEV)/gsKit/lib -L$(PS2DEV)/isjpcm/lib/
EE_LIBS = -lsdlmain -lsdlmixer -lsdl -lgskit -lcdvd -lm -lps2ip -ldebug -lconfig -lmc -lc -lhdd -lfileXio -lpoweroff -lsjpcm
EE_CFLAGS = -DUSE_RWOPS -DHAVE_CONFIG_H -DHAVE_MIXER #-g

all: $(EE_BIN)
	mv $(EE_BIN) bin/

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

run:
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

usbd.s : usbd.irx
	bin2s usbd.irx usbd.s usbd

usbhdfsd.s : usbhdfsd.irx
	bin2s usbhdfsd.irx usbhdfsd.s usbhdfsd
	
SJPCM.s : SJPCM.IRX
	bin2s SJPCM.IRX SJPCM.s SJPCM

freesd.s : freesd-1.01.irx
	bin2s freesd-1.01.irx freesd.s freesd
	
iomanX.s : iomanX.irx
	bin2s iomanX.irx iomanX.s iomanX

fileXio.s : fileXio.irx
	bin2s fileXio.irx fileXio.s fileXio

ps2dev9.s : ps2dev9.irx
	bin2s ps2dev9.irx ps2dev9.s ps2dev9

ps2atad.s : ps2atad.irx
	bin2s ps2atad.irx ps2atad.s ps2atad

ps2hdd.s : ps2hdd.irx
	bin2s ps2hdd.irx ps2hdd.s ps2hdd

ps2fs.s : ps2fs.irx
	bin2s ps2fs.irx ps2fs.s ps2fs

poweroff.s : poweroff.irx
	bin2s poweroff.irx poweroff.s poweroff
	
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
