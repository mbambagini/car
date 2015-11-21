MODULES := lib chassis chassis_test gateway

all:
	@$(foreach MODULE,$(MODULES), cd $(MODULE); make 2>> compile.cmp; cd ../;)

clean:
	@$(foreach MODULE,$(MODULES), cd $(MODULE); make clean; cd ../;)

