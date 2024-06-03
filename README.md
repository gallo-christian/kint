# Kint

Keylogger con architettura server/client scritto in C++17 usando le API di sistema di Windows. Il server supporta solamente una connessione alla volta.

# Progetto

In questa repo e' inclusa una soluzione di Visual Studio che contiene tre progetti: `kint`, `kint_client`, e `kint_common`.

## `kint`

`kint` e' il progetto che contiene il codice del server. Registra tutti gli eventi della tastiera in un buffer (il limite della cronologia e' definito dalla macro `KINT_KEYPRESS_HISTORY_LIMIT`). Se un client e' connesso, il server invia tutti i tasti ricevuti.

## `kint_client`

E' il progetto che contiene il codice del client. Riceve le informazioni inviate dal server e le stampa allo schermo.

## `kint_common`

`kint_common` contiene classi/funzioni che sono comuni in `kint` e `kint_client`. Definisce la struttura delle informazioni di un tasto (`keypress_t`).

# Informazioni ricevute dal server

Le informazioni su un evento sono contenuti nella struttura `keypress_t`, che definisce le seguenti proprieta':
  
  - `std::time_t timestamp`: timestamp di quando e' stato ricevuto l'evento, in secondi dall'epoch UNIX;
  - `DWORD keycode`: [keycode virtuale](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes), usato per convertire tasti senza rappresentazione ASCII leggibile (es. tab, caps-lock, tasto windows);
  - `DWORD scancode`: valore di un tasto definito dall'hardware;
  - `char window_title[128]`: titolo della finestra che era in foreground al momento della pressione del tasto;
  - `char key`: il tasto premuto.

# Licenza

Progetto rilasciato sotto licenza MIT. Vedi [LICENSE](LICENSE)

# Avvertenze legali

**Questo software e' solamente a scopo di educazione personale, non deve essere utilizzato per scopi illegali o non autorizzati. L'autore di questo progetto non sarà responsabile per eventuali danni o conseguenze derivanti dall'uso di questo software.**

>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
