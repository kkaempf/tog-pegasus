ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
    DYNAMIC_LIBRARIES = $(addprefix $(PEGASUS_DEST_LIB_DIR)/$(LIB_PREFIX), \
	$(addsuffix .x, $(LIBRARIES)))
else
    DYNAMIC_LIBRARIES = $(addprefix -L$(PEGASUS_DEST_LIB_DIR) -l, $(LIBRARIES))
endif
