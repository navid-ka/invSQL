all: 
	c++ main.cpp -o inventory -l sqlite3

clean:
	rm inventory
