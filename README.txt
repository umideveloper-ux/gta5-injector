GTA 5 ve benzeri oyunlar için gelişmiş, tespit edilmesi son derece zor bir manual map DLL injector altyapısıdır. Klasik LoadLibrary yöntemleri yerine, PE formatının tüm detaylarını (relocation, import, TLS, entry point) manuel olarak işler. Injection sonrası PE header ve izler bellekten silinir, DLL process modül listesinden kaldırılır. Anti-debug, anti-dump, şifreli yükleme ve stealth injection gibi gelişmiş teknikler için altyapı hazırdır. Kodlar açıklamasız, izsiz ve profesyonel şekilde hazırlanmıştır. Sadece kendi sunucumda ve test ortamlarında kullanmak üzere geliştirilmiştir.

---

## Teknik Detaylar ve Kullanılan Yöntemler

- **Manual Mapping:** DLL, hedef process'e klasik LoadLibrary ile değil, PE header, section'lar, import table, relocation, TLS callback ve entry point işlemleri tamamen manuel olarak belleğe yüklenir. Bu sayede modül, Windows loader'ın izlediği standart yolları atlayarak yüklenir ve tespit edilmesi ciddi şekilde zorlaşır.

- **Relocation & Import Table Çözümü:** DLL'in taşınabilirliği için relocation tablosu ve import tablosu eksiksiz şekilde çözülür. Import edilen fonksiyonlar, hedef process'in adres alanında dinamik olarak resolve edilir ve ilgili thunk'lara yazılır.

- **TLS Callback Handling:** TLS (Thread Local Storage) callback'leri manuel olarak tespit edilip, injection sırasında uygun şekilde tetiklenir. Bu, bazı gelişmiş DLL'lerin doğru çalışması için kritik önemdedir.

- **PEB Manipülasyonu (Unlinking):** DLL, injection sonrası PEB (Process Environment Block) içindeki modül listesinden kaldırılır. Böylece klasik modül tarama teknikleriyle tespit edilmesi engellenir.

- **PE Header ve Section Temizliği:** Injection tamamlandıktan sonra, PE header ve section'lar bellekten sıfırlanır. Bu, memory dump veya forensic analizlerde DLL'in izini sürmeyi neredeyse imkânsız hale getirir.

- **Anti-Debug & Anti-Dump:** Debugger tespiti, hardware breakpoint kontrolü, NtQueryInformationProcess, timing analizleri gibi gelişmiş anti-debug teknikleri uygulanır. Dump alınmasını engellemek için memory patching ve handle hijacking altyapısı mevcuttur.

- **Şifreli DLL Yükleme:** DLL dosyası şifreli olarak saklanabilir, injector tarafından bellekte çözülerek yüklenir. Bu, disk üzerinde analiz ve tespit riskini minimize eder.

- **Stealth Injection (Process Hollowing/Ghosting):** Klasik injection tekniklerinin ötesinde, process hollowing ve process ghosting gibi gelişmiş stealth injection yöntemleri için altyapı hazırlanmıştır.

- **Obfuscation & Randomization:** Kodda gereksiz string, imza, açıklama bulunmaz. Bellek adresleri, section isimleri ve fonksiyon isimleri randomize edilebilir. Signature tabanlı tespitlere karşı dirençlidir.

- **Exception Handling:** Injection sırasında oluşabilecek tüm istisnalar ve hatalar için kapsamlı exception handling mekanizması mevcuttur.

- **Platform Uyumluluğu:** Kod, hem 32-bit hem 64-bit Windows process'leri hedef alacak şekilde tasarlanmıştır. PEB, LDR ve diğer internal Windows yapıları doğrudan kullanılır.

---

## Kullanım:

1. TrainerDLL klasörünü derleyip TrainerDLL.dll dosyasını elde et.
2. Injector klasörünü derleyip Injector.exe dosyasını elde et.
3. Injector.exe'yi şu şekilde çalıştır:
   Injector.exe "GTA5.exe" "C:\\tam\\yol\\TrainerDLL.dll"