# Add driver files to this list
SRC = 	./../driver/internal/cra/si_drv_cra.c         \
	./../driver/internal/ipc/si_drv_ipc.c         \
	./../driver/internal/isp/si_drv_isp.c         \
	./../driver/internal/log/si_sys_log.c         \
	./../driver/internal/malloc/si_sys_malloc.c   \
	./../driver/internal/obj/si_sys_obj.c         \
	./../driver/internal/time/si_sys_time.c       \
	./../driver/sii9777rx/Sii9777RxLib.c          \

OBJ 	= $(notdir $(SRC:.c=.o))
DEP 	= $(notdir $(SRC:.c=.d))
VPATH  	= $(sort $(dir $(SRC))) 
TARGET 	= libsii9777rx.a

CFLAGS =  -O2 -Wall -Werror -MD -MP 
# Add include paths below
CFLAGS += -I. -I./../driver/api -I./../driver/internal -I./../driver/internal/log -I./../driver/internal/obj 
CFLAGS += -I./../driver/internal/malloc -I./../driver/internal/time -I./../driver/internal/cra
CFLAGS += -I./../driver/internal/isp -I./../driver/internal/ipc -I./../platform_example
CFLAGS += -I./../driver/sii9777common -I./../driver/sii9777rx -I./../driver/sii9777mhl

.PHONY: all 
all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJ)

-include $(DEP)

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET) $(DEP)

print-%:
	@echo '$*=$($*)'