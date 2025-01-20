# Proiect: Monitorizarea Câmpurilor Magnetice din Stațiile de Transformare

Proiectul își propune dezvoltarea unui sistem IoT pentru monitorizarea echipamentelor electrice de înaltă tensiune din stațiile de transformare, prin detectarea anomaliilor în câmpurile magnetice generate de acestea. Utilizând senzori cu efect Hall conectați la o placă ESP32, sistemul va colecta date despre intensitatea și variațiile câmpurilor magnetice. Aceste date vor fi transmise wireless către un laptop care acționează ca hub central și, ulterior, vor fi încărcate în Google Cloud Services pentru procesare și analiză avansată. O aplicație dedicată va analiza datele în timp real și va emite alerte atunci când sunt detectate deviații ce pot indica posibile defecțiuni. 

## Structura Proiectului

- **ESP32**: Cod scris în Arduino IDE pentru citirea datelor de la senzori și transmiterea lor către Hub.
- **Hub**: Server Flask pentru procesarea datelor și interacțiunea cu ESP32 și cloud.
- **Aplicația de Vizualizare**: Server Flask pentru descărcarea și vizualizarea datelor stocate în Google Cloud.
- **Cloud**: Integrare cu Google Cloud Storage pentru stocarea datelor.

---

## Configurarea Software-ului

### 1. Codul ESP32 (Arduino IDE)

#### Biblioteci necesare:
- `ArduinoJson`

#### Funcționalitate:
- Citește datele de la senzori.
- Trimite datele către Hub.
- În cazul alertelor, "întrerupe curentul din stația de transformare".

#### Configurare:
- ESP32 trebuie să fie conectat la aceeași rețea Wi-Fi ca Hub-ul.
- Setează IP-ul Hub-ului în codul ESP32 la variabila `serverUrl`.

### 2. Codul Hub-ului - Server Flask (Python)

#### Biblioteci necesare:
- `flask`
- `pyopenssl`

#### Configurarea certificatului SSL:
Rulați următoarea comandă pentru a genera un certificat SSL:
```bash
openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt -days 365
```

#### Funcționalitate:
- Primește datele de la ESP32.
- Preprocesează datele și le trimite către Google Cloud.
- În cazul datelor non-conforme, trimite o comandă către ESP32 pentru a opri curentul electric.

### 3. Codul Aplicației de Vizualizare a Datelor - Server Flask (Python)

#### Biblioteci necesare:
- `flask`

#### Funcționalitate:
- Descarcă periodic datele din Google Cloud Storage.
- Creează o vizualizare live a datelor.
- Include funcționalitate de autentificare.

### 4. Configurarea Rețelei și IP-urilor
- **ESP32**: Conectează dispozitivul la aceeași rețea Wi-Fi ca Hub-ul.
- **Hub-ul**: IP-ul trebuie setat în codul ESP32 la variabila `serverUrl`.

---

## Configurarea Google Cloud Services

1. **Crearea Proiectului**:
   - Accesează consola Google Cloud.
   - Creează un nou proiect și activează API-ul Cloud Storage.

2. **Generarea unui Cont de Serviciu**:
   - Creează un cont de serviciu cu roluri precum „Storage Admin” sau „Storage Object Admin”.
   - Generează o cheie JSON pentru autentificare.

3. **Configurarea Bucket-ului**:
   - Creează un bucket unic pentru stocarea datelor.
   - Atribuie permisiuni contului de serviciu pentru acest bucket.

4. **Integrarea în Aplicație**:
   - Include cheia JSON în aplicația Python pentru autentificare.
   - Inițializează un client pentru interacțiunea cu bucket-ul (încărcare, descărcare, listare fișiere).

---

## Note Importante
- Asigurați-vă că toate dispozitivele sunt conectate la aceeași rețea pentru o comunicare eficientă.
- Păstrați cheia JSON în siguranță; aceasta este cheia de acces la serviciile cloud.

---

### Autor: Vlad Juja
