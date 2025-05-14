# 🚀 GTA5 Manual Map Injector

GTA 5 ve benzeri oyunlar için geliştirilen, tespit edilmesi son derece zor bir **manual map DLL injector** altyapısıdır. Klasik LoadLibrary yöntemleri yerine, PE formatının tüm detaylarını (relocation, import, TLS, entry point) manuel olarak işler. Injection sonrası PE header ve izler bellekten silinir, DLL process modül listesinden kaldırılır. Anti-debug, anti-dump, şifreli yükleme ve stealth injection gibi gelişmiş teknikler için altyapı hazırdır. Kodlar açıklamasız, izsiz ve profesyonel şekilde hazırlanmıştır. Sadece kendi sunucumda ve test ortamlarında kullanmak üzere geliştirilmiştir.Alt yapıyı temsil eder,Yol gösterme amacıyla hazırlanmıştır.

---

## 🛠️ Teknik Detaylar ve Kullanılan Yöntemler

- **Manual Mapping:** DLL, hedef process'e klasik LoadLibrary ile değil, PE header, section'lar, import table, relocation, TLS callback ve entry point işlemleri tamamen manuel olarak belleğe yüklenir.
- **Relocation & Import Table Çözümü:** Relocation ve import tablosu eksiksiz çözülür, import edilen fonksiyonlar dinamik olarak resolve edilir.
- **TLS Callback Handling:** TLS callback'leri manuel olarak tespit edilip injection sırasında tetiklenir.
- **PEB Manipülasyonu (Unlinking):** DLL, injection sonrası PEB içindeki modül listesinden kaldırılır.
- **PE Header ve Section Temizliği:** Injection sonrası PE header ve section'lar bellekten sıfırlanır.
- **Anti-Debug & Anti-Dump:** Debugger tespiti, hardware breakpoint kontrolü, timing analizleri gibi gelişmiş anti-debug teknikleri uygulanır.
- **Şifreli DLL Yükleme:** DLL şifreli olarak saklanabilir, injector bellekte çözer.
- **Stealth Injection:** Process hollowing/ghosting gibi gelişmiş injection teknikleri için altyapı hazır.
- **Obfuscation & Randomization:** Kodda gereksiz string, imza, açıklama yoktur. Signature tabanlı tespitlere karşı dirençlidir.
- **Exception Handling:** Injection sırasında oluşabilecek tüm istisnalar için kapsamlı exception handling mekanizması mevcuttur.
- **Platform Uyumluluğu:** Hem 32-bit hem 64-bit Windows process'leri hedef alınabilir.

---

## 📦 Kullanım

1. `TrainerDLL` klasörünü derleyip `TrainerDLL.dll` dosyasını elde et.
2. `Injector` klasörünü derleyip `Injector.exe` dosyasını elde et.
3. Aşağıdaki komutla inject işlemini başlat:

   ```sh
   Injector.exe "GTA5.exe" "C:\tam\yol\TrainerDLL.dll"
   ```

---

## 🚦 Durumlar & Uyarılar

- 🟢 **Başarılı:** İşlem tamamlandıysa DLL injection gerçekleşmiştir.
- 🟡 **Uyarı:** Sadece kendi sunucunda ve test ortamında kullan.
