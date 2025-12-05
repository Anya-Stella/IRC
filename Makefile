NAME = ircserv

############################################################ DIRECTORY ############################################################
SRC_DIR := src
INC_DIR := include
OBJ_DIR := bin

############################################################### SRCS ##############################################################
SRC := \
		Channel.cpp \
		Utils.cpp \
		Client.cpp \
		Server.cpp \
		CMDS/executeCmds.cpp \
		CMDS/handleCAP.cpp \
		CMDS/handleNICK.cpp \
		CMDS/handlePASS.cpp \
		CMDS/handleUSER.cpp \
		CMDS/handlePING.cpp \
		CMDS/handlePONG.cpp \
		CMDS/handleJOIN.cpp \
		CMDS/handlePART.cpp \
		CMDS/handlePRIVMSG.cpp \
		CMDS/handleNOTICE.cpp \
		CMDS/handleKICK.cpp \
		CMDS/handleQUIT.cpp \
		CMDS/handleINVITE.cpp \
		CMDS/handleTOPIC.cpp \
		CMDS/handleMODE.cpp \
		main.cpp


SRCS      := $(addprefix $(SRC_DIR)/,$(SRC))
OBJS      := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

############################################################## COMPILE ############################################################
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)

############################################################### RULES #############################################################
all: $(NAME)

$(NAME): $(OBJS)
		$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
		@mkdir -p $(dir $@)
		$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
		rm -rf $(OBJ_DIR)

fclean: clean
		rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re