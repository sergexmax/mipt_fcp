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

$(ServerExecutable): $(ServerCExecutable) .tmp_dir .server_tmp_dir

$(ServerCExecutable): $(ServerCObjects) .bin_dir
	$(CC) $(ServerCFlags) $(ServerCObjects) -o $@

$(ServerObjectsDir)%.o: $(ServerSourcesDir)%.c .server_object_dir
	$(CC) $(ServerCFlags) $< -c -o $@

.server_object_dir:
	mkdir -p $(ServerObjectsDir)

.server_tmp_dir:
	mkdir -p $(ServerTmpDir)

$(ClientExecutable): $(ClientCExecutable) .tmp_dir .client_tmp_dir

$(ClientCExecutable): $(ClientCObjects) .bin_dir
	$(CC) $(ClientCFlags) $(ClientCObjects) -o $@

$(ClientObjectsDir)%.o: $(ClientSourcesDir)%.c .client_object_dir
	$(CC) $(ClientCFlags) $< -c -o $@

.client_object_dir:
	mkdir -p $(ClientObjectsDir)

.client_tmp_dir:
	mkdir -p $(ClientTmpDir)

.tmp_dir:
	mkdir -p $(TmpDir)

.bin_dir:
	mkdir -p $(BinDir)

clean:
	rm -rf $(BinDir) $(ServerObjectsDir) $(ClientObjectsDir) $(TmpDir) $(ServerTmpDir) $(ClientTmpDir)
