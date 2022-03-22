TARGET=$(shell basename $$(pwd))
$(TARGET): FORCE
	coddle

FORCE:
