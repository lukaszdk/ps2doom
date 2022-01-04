
# newlib i_main.o
EE_OBJS = am_map.o cosmitoFileIO.o d_items.o d_main.o d_net.o doomdef.o doomstat.o \
dstrings.o f_finale.o f_wipe.o g_game.o hu_lib.o hu_stuff.o  \
i_main.o i_net.o i_sound.o i_system.o i_video.o info.o m_argv.o m_bbox.o \
m_cheat.o m_fixed.o m_menu.o m_misc.o m_random.o m_swap.o mixer_thread.o mixer.o mmus2mid.o p_ceilng.o \
p_doors.o p_enemy.o p_floor.o p_inter.o p_lights.o p_map.o p_maputl.o \
p_mobj.o p_plats.o p_pspr.o p_saveg.o p_setup.o p_sight.o p_spec.o \
p_switch.o p_telept.o p_tick.o p_user.o ps2doom.o r_bsp.o r_data.o r_draw.o \
r_main.o r_plane.o r_segs.o r_sky.o r_things.o s_sound.o sjpcm_rpc.o sounds.o \
st_lib.o st_stuff.o tables.o v_video.o w_wad.o w_mmap.o wi_stuff.o z_zone.o \

EE_BIN = ps2doom.elf
BIN2S = $(PS2SDK)/bin/bin2s
EE_BIN_DIR = bin/ps2doom.elf
EE_INCS = -I$(GSKIT)/include -I$(GSKIT)/ee/dma/include -I$(GSKIT)/ee/gs/include -I$(GSKIT)/ee/toolkit/include -I$(PS2SDK)/ports/include/SDL -I$(PS2SDK)/ports/include -I$(PS2DEV)/isjpcm/include/ 
EE_LDFLAGS = -L$(PS2SDK)/ports/lib -L$(PS2DEV)/gsKit/lib -L$(PS2DEV)/isjpcm/lib/ -L$(PS2SDK)/iop/lib/ -L$(PS2SDK)/ee/lib/
EE_LIBS = -lsdlmain -lsdlmixer -lsdl -lgskit -lcdvd -lm -lps2ip -ldebug -lconfig -lmc -lc -lhdd -lfileXio -lpoweroff -lsjpcm
EE_CFLAGS = -DUSE_RWOPS -DHAVE_CONFIG_H -DHAVE_MIXER -Wall 

all: $(EE_BIN)
	mv $(EE_BIN) bin/

#poweroff Module
poweroff.s: $(PS2SDK)/iop/irx/poweroff.irx
	$(BIN2S) $< $@ poweroff_irx

#IRX Modules
freesio2.s: $(PS2SDK)/iop/irx/freesio2.irx
	$(BIN2S) $< $@ freesio2_irx
	
iomanX.s: $(PS2SDK)/iop/irx/iomanX.irx
	$(BIN2S) $< $@ iomanX_irx 
	
filexio_irx.s: $(PS2SDK)/iop/irx/fileXio.irx
	$(BIN2S) $< $@ filexio_irx

freepad.s: $(PS2SDK)/iop/irx/freepad.irx
	$(BIN2S) $< $@ freepad_irx

mcman_irx.s: $(PS2SDK)/iop/irx/mcman.irx
	$(BIN2S) $< $@ mcman_irx

mcserv_irx.s: $(PS2SDK)/iop/irx/mcserv.irx
	$(BIN2S) $< $@ mcserv_irx

ps2dev9.s: $(PS2SDK)/iop/irx/ps2dev9.irx
	$(BIN2S) $< $@ ps2dev9_irx 

ps2atad: $(PS2SDK)/iop/irx/ps2atad.irx
	$(BIN2S) $< $@ ps2atad_irx

ps2fs_irx.s: $(PS2SDK)/iop/irx/ps2fs-xosd.irx
	$(BIN2S) $< $@ ps2fs_irx

ps2hdd_irx.s: $(PS2SDK)/iop/irx/ps2hdd-xosd.irx
	$(BIN2S) $< $@ ps2hdd_irx

ps2ip-nm.s: $(PS2SDK)/iop/irx/ps2ip-nm.irx
	$(BIN2S) $< $@ ps2ip-nm_irx

ps2ips.s: $(PS2SDK)/iop/irx/ps2ips.irx
	$(BIN2S) $< $@ ps2ips_irx 

netman.s: $(PS2SDK)/iop/irx/netman.irx 
	$(BIN2S) $< $@ netman_irx 

smap.s: $(PS2SDK)/iop/irx/smap.irx 
	$(BIN2S) $< $@ smap_irx 

ps2http.s: bin2s $(PS2SDK)/iop/irx/ps2http.irx 
	$(BIN2S) $< $@ ps2http_irx

usbd_irx.s: $(PS2SDK)/iop/irx/usbd.irx
	$(BIN2S) $< $@ usbd_irx

usbhdfsd_irx.s: $(PS2SDK)/iop/irx/usbhdfsd.irx
	$(BIN2S) $< $@ usb_mass_irx

usbmass_bd.s: $(PS2SDK)/iop/irx/usbmass_bd.irx
	$(BIN2S) $< $@ usbmass_bd_irx
 
isjpcm.s: $(PS2DEV)/isjpcm/bin/isjpcm.irx
	$(BIN2S) $< $@ isjpcm_irx

clean:
	rm -f $(EE_OBJS) $(EE_BIN_DIR)

run:
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
