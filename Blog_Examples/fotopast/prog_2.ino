function doPost(e) {  // Funkce google scriptu prijimajici data metodou HTTPS POST
    var data = Utilities.base64Decode(e.parameters.data);  // Dekoduje prichozi data
    var timeZone = Session.getScriptTimeZone();  // Nastavi se casove pasmo
    var nazevSouboru = Utilities.formatDate(new Date(), timeZone, "yyyyMMdd_HHmmss")+".jpg";  // Sestavi se nazev souboru
    var blob = Utilities.newBlob(data, e.parameters.mimetype, nazevSouboru );  // Vytvori se soubor obrazku

    // Ulozi obrazek na Google Disk
    var folder, folders = DriveApp.getFoldersByName("Fotopast"); // Nazev slozky, do ktere se budou obrazky ukladat
    if (folders.hasNext()) {  // Pokud slozka existuje
        folder = folders.next();  // Pouzije se existujici slozka
    } else {  // Pokud slozka neexistuje
        folder = DriveApp.createFolder("Fotopast");  // Slozka se vytvori
    }
    var file = folder.createFile(blob);  // Ulozi obrazek do slozky
    return ContentService.createTextOutput('Ulozeno')  // Vrati odesilateli text "Ulozeno"
}
