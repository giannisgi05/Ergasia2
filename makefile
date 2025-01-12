CC = gcc                   # Ο compiler που θα χρησιμοποιηθεί (gcc)

OBJ = main.o functions.o   # Τα αρχεία αντικειμένου (.o) που χρειάζονται για τη σύνδεση.
EXEC = eshop               # Το όνομα του εκτελέσιμου προγράμματος.

all: $(EXEC)               # Ο κύριος στόχος του Makefile. Κατά την εκτέλεση της εντολής `make`,
                           # το Makefile εκτελεί τον στόχο `all`, που δημιουργεί το εκτελέσιμο.

$(EXEC): $(OBJ)            # Το εκτελέσιμο πρόγραμμα `eshop` εξαρτάται από τα αρχεία αντικειμένου.
	$(CC) -o $(EXEC) $(OBJ)  
                                       # Χρησιμοποιεί τον `gcc` για να συνδέσει τα αρχεία .o και να παράξει το εκτελέσιμο `eshop`.

main.o: main.c functions.h # Το `main.o` εξαρτάται από τα αρχεία `main.c` και `functions.h`.
	$(CC) -c main.c  	# Μεταγλωττίζει το `main.c` σε `main.o`.

functions.o: functions.c functions.h # Το `functions.o` εξαρτάται από τα `functions.c` και `functions.h`.
	$(CC) -c functions.c	  # Μεταγλωττίζει το `functions.c` σε `functions.o`.

clean:                     
	rm $(OBJ) $(EXEC)    # Διαγραφή όλων των παραγόμενων αρχείων (.o, το εκτελέσιμο).
