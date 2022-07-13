BOARD_PCA10059_DIR = $(BOARD_DIR)/modules

INC += -I$(BOARD_PCA10059_DIR)
CFLAGS += -DBOARD_SPECIFIC_MODULES

SRC_BOARD_MODULES = $(addprefix $(BOARD_PCA10059_DIR)/,\
                      recover_uicr_regout0.c \
                      )

# OBJ += $(addprefix $(BUILD)/, $(SRC_BOARD_MODULES:.c=.o))
OBJ += $(addprefix $(BUILD)/, boards/$(BOARD)/modules/$(notdir $(SRC_BOARD_MODULES:.c=.o)))

