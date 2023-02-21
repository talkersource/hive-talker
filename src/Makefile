BASE_DIR     = /home/james/hive
SRC_DIR      = $(BASE_DIR)/src
BIN_DIR      = $(BASE_DIR)/bin
BINARY_NAME  = hive_d
SERVER_NAME  = "The Hive"
VERSION      = "0.32"
PRIMARY_PORT = 5000
ADNS_DIR     = $(BASE_DIR)/adns-1.0/src

CC      = gcc
CFLAGS  = -rdynamic -ggdb3 -Wall -Iinclude -DLINUX -c -DBASE_DIR='"$(BASE_DIR)"' -DSERVER_NAME='$(SERVER_NAME)' -DVERSION='$(VERSION)' -DPRIMARY_PORT='$(PRIMARY_PORT)'
CMOD    = -fPIC
LD      = gcc
LDFLAGS = -ldl -rdynamic -lcrypt -lm
LDMOD   = -export-dynamic -shared -ggdb3 -Wall

all : $(BIN_DIR)/$(BINARY_NAME) plugins 
	@( echo "Line count :`wc -l *.c plugins/*.c include/*.h | grep total`" ) 
	@( echo "Done" )

$(BIN_DIR)/$(BINARY_NAME) : main.o socket.o parse.o tools.o basecomms.o login.o str.o command.o ccommands.o user.o plugins.o signals.o recovery.o binfiles.o
	@(if ( test -x $(BIN_DIR)/$(BINARY_NAME) ) then (echo "$(BINARY_NAME) exists - overwriting"; rm -f $(BIN_DIR)/$(BINARY_NAME)) fi)
	@( echo "Linking $(BINARY_NAME)" )
#	@( $(LD) $(LDFLAGS) main.o socket.o parse.o tools.o basecomms.o login.o str.o command.o ccommands.o user.o plugins.o signals.o recovery.o binfiles.o $(ADNS_DIR)/libadns.a -o $(BIN_DIR)/$(BINARY_NAME) )
	@( $(LD) $(LDFLAGS) main.o socket.o parse.o tools.o basecomms.o login.o str.o command.o ccommands.o user.o plugins.o signals.o recovery.o binfiles.o -o $(BIN_DIR)/$(BINARY_NAME) )
	@(chmod 775 $(BIN_DIR)/$(BINARY_NAME) )
	@(echo "$(BINARY_NAME) Built") 
	
core : $(BIN_DIR)/$(BINARY_NAME)

main.o : main.c include/core_headers.h include/core_externs.h include/structures.h include/plugin_externs.h
	@( echo "Compiling main.c" )
	@( $(CC) $(CFLAGS) main.c )

socket.o : socket.c 
	@( echo "Compiling socket.c" )
	@( $(CC) $(CFLAGS) socket.c )

parse.o : parse.c 
	@( echo "Compiling parse.c" )
	@( $(CC) $(CFLAGS) parse.c )

tools.o : tools.c 
	@( echo "Compiling tools.c" )
	@( $(CC) $(CFLAGS) tools.c )

basecomms.o : basecomms.c 
	@( echo "Compiling basecomms.c" )
	@( $(CC) $(CFLAGS) basecomms.c )

login.o : login.c 
	@( echo "Compiling login.c" )
	@( $(CC) $(CFLAGS) login.c )

str.o : str.c include/str.h
	@( echo "Compiling str.c" )
	@( $(CC) $(CFLAGS) str.c )

command.o : command.c 
	@( echo "Compiling command.c" )
	@( $(CC) $(CFLAGS) command.c )

ccommands.o : ccommands.c 
	@( echo "Compiling ccommands.c" )
	@( $(CC) $(CFLAGS) ccommands.c )

user.o : user.c 
	@( echo "Compiling user.c" )
	@( $(CC) $(CFLAGS) user.c )

plugins.o : plugins.c 
	@( echo "Compiling plugins.c" )
	@( $(CC) $(CFLAGS) plugins.c )

signals.o : signals.c 
	@( echo "Compiling signals.c" )
	@( $(CC) $(CFLAGS) signals.c )

recovery.o : recovery.c 
	@( echo "Compiling recovery.c" )
	@( $(CC) $(CFLAGS) recovery.c )

binfiles.o : binfiles.c 
	@( echo "Compiling binfiles.c" )
	@( $(CC) $(CFLAGS) binfiles.c )

plugins : example.so textfiles.so system.so history.so groups.so help.so gamebase.so info.so save.so admin.so private.so user_settings.so profiles.so bships.so elite.so prophecy.so noop.so
	@( echo "Plugins Built" )

example.so : plugins/example.c
	@( echo "Building plugin example.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c plugins/example.c )
	@( $(LD) $(LDMOD) -o example.so example.o )
	@( mv example.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/example.so )

textfiles.so : plugins/textfiles.c
	@( echo "Building plugin textfiles.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c plugins/textfiles.c )
	@( $(LD) $(LDMOD) -o textfiles.so textfiles.o )
	@( mv textfiles.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/textfiles.so )

system.so : plugins/system.c
	@( echo "Building plugin system.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c plugins/system.c )
	@( $(LD) $(LDMOD) -o system.so system.o )
	@( mv system.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/system.so )

history.so : plugins/history.c
	@( echo "Building plugin history.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/history.c )
	@( $(LD) $(LDMOD) -o history.so history.o )
	@( mv history.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/history.so )

groups.so : plugins/groups.c plugins/include/groups.h
	@( echo "Building plugin groups.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/groups.c )
	@( $(LD) $(LDMOD) -o groups.so groups.o )
	@( mv groups.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/groups.so )

help.so : plugins/help.c plugins/include/help.h
	@( echo "Building plugin help.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/help.c )
	@( $(LD) $(LDMOD) -o help.so help.o )
	@( mv help.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/help.so )

gamebase.so : plugins/gamebase.c
	@( echo "Building plugin gamebase.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/gamebase.c )
	@( $(LD) $(LDMOD) -o gamebase.so gamebase.o )
	@( mv gamebase.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/gamebase.so )

info.so : plugins/info.c
	@( echo "Building plugin info.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/info.c )
	@( $(LD) $(LDMOD) -o info.so info.o )
	@( mv info.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/info.so )

save.so : plugins/save.c
	@( echo "Building plugin save.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/save.c )
	@( $(LD) $(LDMOD) -o save.so save.o )
	@( mv save.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/save.so )

admin.so : plugins/admin.c
	@( echo "Building plugin admin.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/admin.c )
	@( $(LD) $(LDMOD) -o admin.so admin.o )
	@( mv admin.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/admin.so )

private.so : plugins/private.c
	@( echo "Building plugin private.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/private.c )
	@( $(LD) $(LDMOD) -o private.so private.o )
	@( mv private.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/private.so )

user_settings.so : plugins/user_settings.c
	@( echo "Building plugin user_settings.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/user_settings.c )
	@( $(LD) $(LDMOD) -o user_settings.so user_settings.o )
	@( mv user_settings.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/user_settings.so )

profiles.so : plugins/profiles.c
	@( echo "Building plugin profiles.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/profiles.c )
	@( $(LD) $(LDMOD) -o profiles.so profiles.o )
	@( mv profiles.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/profiles.so )

bships.so : plugins/bships.c
	@( echo "Building plugin bships.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/bships.c )
	@( $(LD) $(LDMOD) -o bships.so bships.o )
	@( mv bships.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/bships.so )

quiz.so : plugins/quiz.c
	@( echo "Building plugin quiz.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/quiz.c )
	@( $(LD) $(LDMOD) -o quiz.so quiz.o )
	@( mv quiz.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/quiz.so )

elite.so : plugins/elite.c plugins/include/elite.h
	@( echo "Building plugin elite.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/elite.c )
	@( $(LD) $(LDMOD) -o elite.so elite.o )
	@( mv elite.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/elite.so )

prophecy.so : plugins/prophecy.c
	@( echo "Building plugin prophecy.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c -Iplugins/include plugins/prophecy.c )
	@( $(LD) $(LDMOD) -o prophecy.so prophecy.o )
	@( mv prophecy.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/prophecy.so )

noop.so : plugins/noop.c
	@( echo "Building plugin noop.so" )
	@( $(CC) $(CFLAGS) $(CMOD) -c plugins/noop.c )
	@( $(LD) $(LDMOD) -o noop.so noop.o )
	@( mv noop.so $(BIN_DIR)/plugins )
	@( chmod 775 $(BIN_DIR)/plugins/noop.so )

clean :            
	@( echo "Cleaning ..." )
	@( rm -f *.o )       
	@( rm -f $(BIN_DIR)/$(BINARY_NAME) )
	@( rm -f $(BIN_DIR)/plugins/*.so )
	@( echo "Done" )

dist : clean
	@( echo "Making Distribution" )
	rm -rf $(BASE_DIR)/hive
	mkdir $(BASE_DIR)/hive
	cp -R $(BASE_DIR)/License $(BASE_DIR)/hive
	cp -R $(BASE_DIR)/README $(BASE_DIR)/hive 
	cp -R $(BASE_DIR)/bin $(BASE_DIR)/hive
	cp -R $(BASE_DIR)/help $(BASE_DIR)/hive
	cp -R $(BASE_DIR)/src $(BASE_DIR)/hive
	cp -R $(BASE_DIR)/textfiles $(BASE_DIR)/hive
	mkdir $(BASE_DIR)/hive/data
	mkdir $(BASE_DIR)/hive/data/users
	touch $(BASE_DIR)/hive/data/userids
	mkdir $(BASE_DIR)/hive/logs
 

