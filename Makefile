CC = cc

ServerCFlags = -Wall -lpthread
ClientCFlags = -Wall

SourcesDir = src/
IncludesDir = include/
ObjectsDir = obj/
BinDir = bin/
TmpDir = tmp/

ServerDir = server/
ServerSourcesDir = $(addprefix $(ServerDir), $(SourcesDir))
ServerObjectsDir = $(addprefix $(ServerDir), $(ObjectsDir))
ServerTmpDir = $(addprefix $(ServerDir), $(TmpDir))
ServerSources = server.c controller.c communicator.c
ServerObjects = $(ServerSources:.c=.o)
ServerCObjects = $(addprefix $(ServerObjectsDir), $(ServerObjects))
ServerExecutable = server
ServerCExecutable = $(addprefix $(BinDir), $(ServerExecutable))

ClientDir = client/
ClientSourcesDir = $(addprefix $(ClientDir), $(SourcesDir))
ClientObjectsDir = $(addprefix $(ClientDir), $(ObjectsDir))
ClientTmpDir = $(addprefix $(ClientDir), $(TmpDir))
ClientSources = client.c
ClientObjects = $(ClientSources:.c=.o)
ClientCObjects = $(addprefix $(ClientObjectsDir), $(ClientObjects))
ClientExecutable = client
ClientCExecutable = $(addprefix $(BinDir), $(ClientExecutable))

all: $(ServerExecutable) $(ClientExecutable)

$(ServerExecutable): $(ServerCExecutable)

$(ServerCExecutable): $(ServerCObjects) .bin_dir
	$(CC) $(ServerCFlags) $(ServerCObjects) -o $@

$(ServerObjectsDir)%.o: $(ServerSourcesDir)%.c .server_object_dir
	$(CC) $(ServerCFlags) $< -c -o $@

.server_object_dir:
	mkdir -p $(ServerObjectsDir)

$(ClientExecutable): $(ClientCExecutable)

$(ClientCExecutable): $(ClientCObjects) .bin_dir
	$(CC) $(ClientCFlags) $(ClientCObjects) -o $@

$(ClientObjectsDir)%.o: $(ClientSourcesDir)%.c .client_object_dir
	$(CC) $(ClientCFlags) $< -c -o $@

.client_object_dir:
	mkdir -p $(ClientObjectsDir)

.bin_dir:
	mkdir -p $(BinDir)

clean:
	rm -rf $(BinDir) $(ServerObjectsDir) $(ClientObjectsDir) $(TmpDir) $(ServerTmpDir) $(ClientTmpDir)
