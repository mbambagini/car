MODULES := lib chassis chassis_test gateway

all:
	$(foreach MODULE,$(MODULES), cd $(MODULE); make; cd ../;)

clean:
	$(foreach MODULE,$(MODULES), cd $(MODULE); make clean; cd ../;)

