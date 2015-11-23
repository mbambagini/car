MODULES := lib chassis chassis_test gateway

all:
	@rm -f sizes
	$(foreach MODULE,$(MODULES), cd $(MODULE); make; cd ../;)
	@cat sizes

clean:
	@rm -f sizes
	$(foreach MODULE,$(MODULES), cd $(MODULE); make clean; cd ../;)
