#!/usr/bin/env python3
import os
import subprocess
import xml.etree.ElementTree as ET

LANGUAGES = {
    "it": "Italian",
    "en": "English",
    "es": "Spanish",
    "fr": "French",
    "de": "German",
    "pt": "Portuguese",
    "ru": "Russian",
    "zh_CN": "Chinese (Simplified)",
    "zh_TW": "Chinese (Traditional)",
    "ja": "Japanese",
    "ko": "Korean",
    "ar": "Arabic",
    "hi": "Hindi",
    "nl": "Dutch",
    "pl": "Polish",
    "tr": "Turkish",
    "sv": "Swedish",
    "el": "Greek",
    "cs": "Czech",
    "hu": "Hungarian",
    "uk": "Ukrainian",
    "id": "Indonesian"
}

TRANSLATIONS = {
    # Ribbon & General
    "File": {
        "it": "File", "es": "Archivo", "fr": "Fichier", "de": "Datei", "pt": "Arquivo", "ru": "Файл",
        "zh_CN": "文件", "zh_TW": "檔案", "ja": "ファイル", "ko": "파일", "ar": "ملف", "hi": "फ़ाइल",
        "nl": "Bestand", "pl": "Plik", "tr": "Dosya", "sv": "Arkiv", "el": "Αρχείο", "cs": "Soubor",
        "hu": "Fájl", "uk": "Файл", "id": "Berkas"
    },
    "Home": {
        "it": "Home", "es": "Inicio", "fr": "Accueil", "de": "Start", "pt": "Início", "ru": "Главная",
        "zh_CN": "开始", "zh_TW": "常用", "ja": "ホーム", "ko": "홈", "ar": "الصفحة الرئيسية", "hi": "होम",
        "nl": "Start", "pl": "Narzędzia główne", "tr": "Giriş", "sv": "Start", "el": "Κεντρική", "cs": "Domů",
        "hu": "Kezdőlap", "uk": "Основне", "id": "Beranda"
    },
    "View": {
        "it": "Visualizza", "es": "Ver", "fr": "Affichage", "de": "Ansicht", "pt": "Exibir", "ru": "Вид",
        "zh_CN": "查看", "zh_TW": "檢視", "ja": "表示", "ko": "보기", "ar": "عرض", "hi": "देखें",
        "nl": "Beeld", "pl": "Widok", "tr": "Görünüm", "sv": "Visa", "el": "Προβολή", "cs": "Zobrazení",
        "hu": "Nézet", "uk": "Вигляд", "id": "Tampilan"
    },
    "New": {
        "it": "Nuovo", "es": "Nuevo", "fr": "Nouveau", "de": "Neu", "pt": "Novo", "ru": "Создать",
        "zh_CN": "新建", "zh_TW": "新增", "ja": "新規作成", "ko": "새로 만들기", "ar": "جديد", "hi": "नया",
        "nl": "Nieuw", "pl": "Nowy", "tr": "Yeni", "sv": "Nytt", "el": "Δημιουργία", "cs": "Nový",
        "hu": "Új", "uk": "Створити", "id": "Baru"
    },
    "Open...": {
        "it": "Apri...", "es": "Abrir...", "fr": "Ouvrir...", "de": "Öffnen...", "pt": "Abrir...", "ru": "Открыть...",
        "zh_CN": "打开...", "zh_TW": "開啟...", "ja": "開く...", "ko": "열기...", "ar": "فتح...", "hi": "खोलें...",
        "nl": "Openen...", "pl": "Otwórz...", "tr": "Aç...", "sv": "Öppna...", "el": "Άνοιγμα...", "cs": "Otevřít...",
        "hu": "Megnyitás...", "uk": "Відкрити...", "id": "Buka..."
    },
    "Save": {
        "it": "Salva", "es": "Guardar", "fr": "Enregistrer", "de": "Speichern", "pt": "Salvar", "ru": "Сохранить",
        "zh_CN": "保存", "zh_TW": "儲存", "ja": "上書き保存", "ko": "저장", "ar": "حفظ", "hi": "सहेजें",
        "nl": "Opslaan", "pl": "Zapisz", "tr": "Kaydet", "sv": "Spara", "el": "Αποθήκευση", "cs": "Uložit",
        "hu": "Mentés", "uk": "Зберегти", "id": "Simpan"
    },
    "Save As...": {
        "it": "Salva con nome...", "es": "Guardar como...", "fr": "Enregistrer sous...", "de": "Speichern unter...",
        "pt": "Salvar como...", "ru": "Сохранить как...", "zh_CN": "另存为...", "zh_TW": "另存新檔...",
        "ja": "名前を付けて保存...", "ko": "다른 이름으로 저장...", "ar": "حفظ باسم...", "hi": "ऐसे सहेजें...",
        "nl": "Opslaan als...", "pl": "Zapisz jako...", "tr": "Farklı Kaydet...", "sv": "Spara som...",
        "el": "Αποθήκευση ως...", "cs": "Uložit jako...", "hu": "Mentés másként...", "uk": "Зберегти як...", "id": "Simpan Sebagai..."
    },
    "Print...": {
        "it": "Stampa...", "es": "Imprimir...", "fr": "Imprimer...", "de": "Drucken...", "pt": "Imprimir...",
        "ru": "Печать...", "zh_CN": "打印...", "zh_TW": "列印...", "ja": "印刷...", "ko": "인쇄...",
        "ar": "طباعة...", "hi": "प्रिंट...", "nl": "Afdrukken...", "pl": "Drukuj...", "tr": "Yazdır...",
        "sv": "Skriv ut...", "el": "Εκτύπωση...", "cs": "Tisk...", "hu": "Nyomtatás...", "uk": "Друк...", "id": "Cetak..."
    },
    "Quick Print": {
        "it": "Stampa rapida", "es": "Impresión rápida", "fr": "Impression rapide", "de": "Schnelldruck",
        "pt": "Impressão Rápida", "ru": "Быстрая печать", "zh_CN": "快速打印", "zh_TW": "快速列印",
        "ja": "クイック印刷", "ko": "빠른 인쇄", "ar": "طباعة سريعة", "hi": "त्वरित प्रिंट",
        "nl": "Snel afdrukken", "pl": "Szybkie drukowanie", "tr": "Hızlı Yazdır", "sv": "Snabb utskrift",
        "el": "Γρήγορη εκτύπωση", "cs": "Rychlý tisk", "hu": "Gyorsnyomtatás", "uk": "Швидкий друк", "id": "Cetak Cepat"
    },
    "Print Preview": {
        "it": "Anteprima di stampa", "es": "Vista previa de impresión", "fr": "Aperçu avant impression",
        "de": "Seitenansicht", "pt": "Visualizar Impressão", "ru": "Предварительный просмотр",
        "zh_CN": "打印预览", "zh_TW": "預覽列印", "ja": "印刷プレビュー", "ko": "인쇄 미리 보기",
        "ar": "معاينة الطباعة", "hi": "प्रिंट पूर्वावलोकन", "nl": "Afdrukvoorbeeld", "pl": "Podgląd wydruku",
        "tr": "Baskı Önizleme", "sv": "Förhandsgranska", "el": "Προεπισκόπηση εκτύπωσης", "cs": "Náhled tisku",
        "hu": "Nyomtatási kép", "uk": "Попередній перегляд", "id": "Pratinjau Cetak"
    },
    "Page Setup...": {
        "it": "Imposta pagina...", "es": "Configurar página...", "fr": "Mise en page...", "de": "Seite einrichten...",
        "pt": "Configurar Página...", "ru": "Параметры страницы...", "zh_CN": "页面设置...", "zh_TW": "版面設定...",
        "ja": "ページ設定...", "ko": "페이지 설정...", "ar": "إعداد الصفحة...", "hi": "पृष्ठ सेटअप...",
        "nl": "Pagina-instelling...", "pl": "Ustawienia strony...", "tr": "Sayfa Yapısı...", "sv": "Utskriftsformat...",
        "el": "Διαμόρφωση σελίδας...", "cs": "Vzhled stránky...", "hu": "Oldalbeállítás...", "uk": "Параметри сторінки...", "id": "Pengaturan Halaman..."
    },
    "Send in email": {
        "it": "Invia per posta elettronica", "es": "Enviar en correo electrónico", "fr": "Envoyer par e-mail",
        "de": "In E-Mail senden", "pt": "Enviar por email", "ru": "Отправить по почте",
        "zh_CN": "通过电子邮件发送", "zh_TW": "以電子郵件傳送", "ja": "電子メールで送信", "ko": "전자 메일로 보내기",
        "ar": "إرسال بالبريد الإلكتروني", "hi": "ईमेल में भेजें", "nl": "Verzenden via e-mail",
        "pl": "Wyślij w wiadomości e-mail", "tr": "E-posta ile gönder", "sv": "Skicka i e-post",
        "el": "Αποστολή με μήνυμα ηλεκτρονικού ταχυδρομείου", "cs": "Odeslat e-mailem", "hu": "Küldés e-mailben",
        "uk": "Надіслати електронною поштою", "id": "Kirim di email"
    },
    "About WordPad": {
        "it": "Informazioni su WordPad", "es": "Acerca de WordPad", "fr": "À propos de WordPad", "de": "Info über WordPad",
        "pt": "Sobre o WordPad", "ru": "О программе WordPad", "zh_CN": "关于 WordPad", "zh_TW": "關於 WordPad",
        "ja": "WordPad について", "ko": "WordPad 정보", "ar": "حول WordPad", "hi": "WordPad के बारे में",
        "nl": "Info over WordPad", "pl": "WordPad - informacje", "tr": "WordPad Hakkında", "sv": "Om WordPad",
        "el": "Σχετικά με το WordPad", "cs": "O aplikaci WordPad", "hu": "A WordPad névjegye", "uk": "Про WordPad", "id": "Tentang WordPad"
    },
    "Exit": {
        "it": "Esci", "es": "Salir", "fr": "Quitter", "de": "Beenden", "pt": "Sair", "ru": "Выход",
        "zh_CN": "退出", "zh_TW": "結束", "ja": "終了", "ko": "끝내기", "ar": "خروج", "hi": "बाहर निकलें",
        "nl": "Afsluiten", "pl": "Zakończ", "tr": "Çıkış", "sv": "Avsluta", "el": "Έξοδος", "cs": "Konec",
        "hu": "Kilépés", "uk": "Вихід", "id": "Keluar"
    },
    "Clipboard": {
        "it": "Appunti", "es": "Portapapeles", "fr": "Presse-papiers", "de": "Zwischenablage", "pt": "Área de Transferência",
        "ru": "Буфер обмена", "zh_CN": "剪贴板", "zh_TW": "剪貼簿", "ja": "クリップボード", "ko": "클립보드",
        "ar": "الحافظة", "hi": "क्लिपबोर्ड", "nl": "Klembord", "pl": "Schowek", "tr": "Pano",
        "sv": "Urklipp", "el": "Πρόχειρο", "cs": "Schránka", "hu": "Vágólap", "uk": "Буфер обміну", "id": "Papan Klip"
    },
    "Paste": {
        "it": "Incolla", "es": "Pegar", "fr": "Coller", "de": "Einfügen", "pt": "Colar", "ru": "Вставить",
        "zh_CN": "粘贴", "zh_TW": "貼上", "ja": "貼り付け", "ko": "붙여넣기", "ar": "لصق", "hi": "पेस्ट करें",
        "nl": "Plakken", "pl": "Wklej", "tr": "Yapıştır", "sv": "Klistra in", "el": "Επικόλληση", "cs": "Vložit",
        "hu": "Beillesztés", "uk": "Вставити", "id": "Tempel"
    },
    "Paste Special...": {
        "it": "Incolla speciale...", "es": "Pegado especial...", "fr": "Collage spécial...", "de": "Inhalte einfügen...",
        "pt": "Colar especial...", "ru": "Специальная вставка...", "zh_CN": "选择性粘贴...", "zh_TW": "選擇性貼上...",
        "ja": "形式を選択して貼り付け...", "ko": "선택하여 붙여넣기...", "ar": "لصق خاص...", "hi": "विशेष पेस्ट करें...",
        "nl": "Plakken speciaal...", "pl": "Wklej specjalnie...", "tr": "Özel Yapıştır...", "sv": "Klistra in special...",
        "el": "Ειδική επικόλληση...", "cs": "Vložit jinak...", "hu": "Irányított beillesztés...", "uk": "Спеціальна вставка...", "id": "Tempel Khusus..."
    },
    "Cut": {
        "it": "Taglia", "es": "Cortar", "fr": "Couper", "de": "Ausschneiden", "pt": "Recortar", "ru": "Вырезать",
        "zh_CN": "剪切", "zh_TW": "剪下", "ja": "切り取り", "ko": "잘라내기", "ar": "قص", "hi": "कट करें",
        "nl": "Knippen", "pl": "Wytnij", "tr": "Kes", "sv": "Klipp ut", "el": "Αποκοπή", "cs": "Vyjmout",
        "hu": "Kivágás", "uk": "Вирізати", "id": "Potong"
    },
    "Copy": {
        "it": "Copia", "es": "Copiar", "fr": "Copier", "de": "Kopieren", "pt": "Copiar", "ru": "Копировать",
        "zh_CN": "复制", "zh_TW": "複製", "ja": "コピー", "ko": "복사", "ar": "نسخ", "hi": "कॉपी करें",
        "nl": "Kopiëren", "pl": "Kopiuj", "tr": "Kopyala", "sv": "Kopiera", "el": "Αντιγραφή", "cs": "Kopírovat",
        "hu": "Másolás", "uk": "Копіювати", "id": "Salin"
    },
    "Font": {
        "it": "Carattere", "es": "Fuente", "fr": "Police", "de": "Schriftart", "pt": "Fonte", "ru": "Шрифт",
        "zh_CN": "字体", "zh_TW": "字型", "ja": "フォント", "ko": "글꼴", "ar": "خط", "hi": "फ़ॉन्ट",
        "nl": "Lettertype", "pl": "Czcionka", "tr": "Yazı Tipi", "sv": "Teckensnitt", "el": "Γραμματοσειρά", "cs": "Písmo",
        "hu": "Betűtípus", "uk": "Шрифт", "id": "Font"
    },
    "Grow font": {
        "it": "Ingrandisci carattere", "es": "Aumentar tamaño de fuente", "fr": "Agrandir la police", "de": "Schrift vergrößern",
        "pt": "Aumentar fonte", "ru": "Увеличить шрифт", "zh_CN": "增大字号", "zh_TW": "放大字型",
        "ja": "フォントの拡大", "ko": "글꼴 크기 늘리기", "ar": "تكبير الخط", "hi": "फ़ॉन्ट आकार बढ़ाएं",
        "nl": "Lettertype vergroten", "pl": "Zwiększ czcionkę", "tr": "Yazı tipini büyüt", "sv": "Öka teckenstorlek",
        "el": "Μεγέθυνση γραμματοσειράς", "cs": "Zvětšit písmo", "hu": "Betűméret növelése", "uk": "Збільшити шрифт", "id": "Perbesar Font"
    },
    "Shrink font": {
        "it": "Riduci carattere", "es": "Reducir tamaño de fuente", "fr": "Réduire la police", "de": "Schrift verkleinern",
        "pt": "Reduzir fonte", "ru": "Уменьшить шрифт", "zh_CN": "缩小字号", "zh_TW": "縮小字型",
        "ja": "フォントの縮小", "ko": "글꼴 크기 줄이기", "ar": "تصغير الخط", "hi": "फ़ॉन्ट आकार घटाएं",
        "nl": "Lettertype verkleinen", "pl": "Zmniejsz czcionkę", "tr": "Yazı tipini küçült", "sv": "Minska teckenstorlek",
        "el": "Σμίκρυνση γραμματοσειράς", "cs": "Zmenšit písmo", "hu": "Betűméret csökkentése", "uk": "Зменшити шрифт", "id": "Perkecil Font"
    },
    "Bold": {
        "it": "Grassetto", "es": "Negrita", "fr": "Gras", "de": "Fett", "pt": "Negrito", "ru": "Полужирный",
        "zh_CN": "加粗", "zh_TW": "粗體", "ja": "太字", "ko": "굵게", "ar": "غامق", "hi": "बोल्ड",
        "nl": "Vet", "pl": "Pogrubienie", "tr": "Kalın", "sv": "Fet", "el": "Έντονα", "cs": "Tučné",
        "hu": "Félkövér", "uk": "Напівжирний", "id": "Tebal"
    },
    "Italic": {
        "it": "Corsivo", "es": "Cursiva", "fr": "Italique", "de": "Kursiv", "pt": "Itálico", "ru": "Курсив",
        "zh_CN": "倾斜", "zh_TW": "斜體", "ja": "斜体", "ko": "기울임꼴", "ar": "مائل", "hi": "इटैलिक",
        "nl": "Cursief", "pl": "Kursywa", "tr": "İtalik", "sv": "Kursiv", "el": "Πλάγια", "cs": "Kurzíva",
        "hu": "Dőlt", "uk": "Курсив", "id": "Miring"
    },
    "Underline": {
        "it": "Sottolineato", "es": "Subrayado", "fr": "Souligné", "de": "Unterstrichen", "pt": "Sublinhado", "ru": "Подчеркнутый",
        "zh_CN": "下划线", "zh_TW": "底線", "ja": "下線", "ko": "밑줄", "ar": "تسطير", "hi": "रेखांकन",
        "nl": "Onderstrepen", "pl": "Podkreślenie", "tr": "Altı Çizili", "sv": "Understruken", "el": "Υπογράμμιση", "cs": "Podtržené",
        "hu": "Aláhúzott", "uk": "Підкреслений", "id": "Garis Bawah"
    },
    "Strikethrough": {
        "it": "Barrato", "es": "Tachado", "fr": "Barré", "de": "Durchgestrichen", "pt": "Tachado", "ru": "Зачеркнутый",
        "zh_CN": "删除线", "zh_TW": "刪除線", "ja": "取り消し線", "ko": "취소선", "ar": "يتوسطه خط", "hi": "स्ट्राइकथ्रू",
        "nl": "Doorhalen", "pl": "Przekreślenie", "tr": "Üstü Çizili", "sv": "Genomstruken", "el": "Διακριτή διαγραφή", "cs": "Přeškrtnuté",
        "hu": "Áthúzott", "uk": "Закреслений", "id": "Coret"
    },
    "Subscript": {
        "it": "Pedice", "es": "Subíndice", "fr": "Indice", "de": "Tiefgestellt", "pt": "Subscrito", "ru": "Подстрочный",
        "zh_CN": "下标", "zh_TW": "下標", "ja": "下付き", "ko": "아래 첨자", "ar": "منخفض", "hi": "सबस्क्रिप्ट",
        "nl": "Subscript", "pl": "Indeks dolny", "tr": "Alt Simge", "sv": "Nedsänkt", "el": "Δείκτης", "cs": "Dolní index",
        "hu": "Alsó index", "uk": "Нижній індекс", "id": "Subskrip"
    },
    "Superscript": {
        "it": "Apice", "es": "Superíndice", "fr": "Exposant", "de": "Hochgestellt", "pt": "Sobrescrito", "ru": "Надстрочный",
        "zh_CN": "上标", "zh_TW": "上標", "ja": "上付き", "ko": "위 첨자", "ar": "مرتفع", "hi": "सुपरस्क्रिप्ट",
        "nl": "Superscript", "pl": "Indeks górny", "tr": "Üst Simge", "sv": "Upphöjd", "el": "Εκθέτης", "cs": "Horní index",
        "hu": "Felső index", "uk": "Верхній індекс", "id": "Superskrip"
    },
    "Text color": {
        "it": "Colore testo", "es": "Color de texto", "fr": "Couleur du texte", "de": "Textfarbe", "pt": "Cor do texto",
        "ru": "Цвет текста", "zh_CN": "文本颜色", "zh_TW": "文字色彩", "ja": "テキストの色", "ko": "텍스트 색",
        "ar": "لون النص", "hi": "पाठ का रंग", "nl": "Tekstkleur", "pl": "Kolor tekstu", "tr": "Metin Rengi",
        "sv": "Textfärg", "el": "Χρώμα κειμένου", "cs": "Barva textu", "hu": "Szövegszín", "uk": "Колір тексту", "id": "Warna Teks"
    },
    "Text highlight color": {
        "it": "Colore evidenziatore", "es": "Color de resaltado", "fr": "Couleur de surbrillance", "de": "Texthervorhebungsfarbe",
        "pt": "Cor de realce do texto", "ru": "Цвет выделения текста", "zh_CN": "文本突出显示颜色", "zh_TW": "文字醒目提示色彩",
        "ja": "テキストの強調表示色", "ko": "텍스트 강조 색", "ar": "لون تمييز النص", "hi": "हाइलाइट रंग",
        "nl": "Markeringskleur voor tekst", "pl": "Kolor wyróżnienia tekstu", "tr": "Metin Vurgu Rengi", "sv": "Framhävningsfärg",
        "el": "Χρώμα επισήμανσης κειμένου", "cs": "Barva zvýraznění textu", "hu": "Kiemelőszín", "uk": "Колір виділення тексту", "id": "Warna Sorotan Teks"
    },
    "Paragraph": {
        "it": "Paragrafo", "es": "Párrafo", "fr": "Paragraphe", "de": "Absatz", "pt": "Parágrafo", "ru": "Абзац",
        "zh_CN": "段落", "zh_TW": "段落", "ja": "段落", "ko": "단락", "ar": "فقرة", "hi": "अनुच्छेद",
        "nl": "Alinea", "pl": "Akapit", "tr": "Paragraf", "sv": "Stycke", "el": "Παράγραφος", "cs": "Odstavec",
        "hu": "Bekezdés", "uk": "Абзац", "id": "Paragraf"
    },
    "Decrease indent": {
        "it": "Riduci rientro", "es": "Disminuir sangría", "fr": "Diminuer le retrait", "de": "Einzug verringern",
        "pt": "Diminuir recuo", "ru": "Уменьшить отступ", "zh_CN": "减少缩进", "zh_TW": "減少縮排",
        "ja": "インデントを減らす", "ko": "내어쓰기", "ar": "إنقاص المسافة البادئة", "hi": "इंडेंट घटाएं",
        "nl": "Inspringing verkleinen", "pl": "Zmniejsz wcięcie", "tr": "Girintiyi Azalt", "sv": "Minska indrag",
        "el": "Μείωση εσοχής", "cs": "Zmenšit odsazení", "hu": "Behúzás csökkentése", "uk": "Зменшити відступ", "id": "Kurangi Inden"
    },
    "Increase indent": {
        "it": "Aumenta rientro", "es": "Aumentar sangría", "fr": "Augmenter le retrait", "de": "Einzug vergrößern",
        "pt": "Aumentar recuo", "ru": "Увеличить отступ", "zh_CN": "增加缩进", "zh_TW": "增加縮排",
        "ja": "インデントを増やす", "ko": "들여쓰기", "ar": "زيادة المسافة البادئة", "hi": "इंडेंट बढ़ाएं",
        "nl": "Inspringing vergroten", "pl": "Zwiększ wcięcie", "tr": "Girintiyi Artır", "sv": "Öka indrag",
        "el": "Αύξηση εσοχής", "cs": "Zvětšit odsazení", "hu": "Behúzás növelése", "uk": "Збільшити відступ", "id": "Tambah Inden"
    },
    "Start a list": {
        "it": "Elenchi puntati", "es": "Iniciar una lista", "fr": "Démarrer une liste", "de": "Liste beginnen",
        "pt": "Iniciar uma lista", "ru": "Начать список", "zh_CN": "启动列表", "zh_TW": "開始清單",
        "ja": "リストの開始", "ko": "목록 시작", "ar": "بدء قائمة", "hi": "सूची शुरू करें",
        "nl": "Een lijst beginnen", "pl": "Rozpocznij listę", "tr": "Liste Başlat", "sv": "Starta en lista",
        "el": "Έναρξη λίστας", "cs": "Vytvořit seznam", "hu": "Lista indítása", "uk": "Створити список", "id": "Mulai Daftar"
    },
    "Line spacing": {
        "it": "Interlinea", "es": "Espaciado entre líneas", "fr": "Interligne", "de": "Zeilenabstand",
        "pt": "Espaçamento de linha", "ru": "Междустрочный интервал", "zh_CN": "行距", "zh_TW": "行距",
        "ja": "行間", "ko": "줄 간격", "ar": "تباعد الأسطر", "hi": "पंक्ति रिक्ति",
        "nl": "Regelafstand", "pl": "Interlinia", "tr": "Satır Aralığı", "sv": "Radavstånd",
        "el": "Διάστιχο", "cs": "Řádkování", "hu": "Sorköz", "uk": "Міжрядковий інтервал", "id": "Penspasian Baris"
    },
    "Align text left": {
        "it": "Allinea a sinistra", "es": "Alinear texto a la izquierda", "fr": "Aligner le texte à gauche",
        "de": "Text linksbündig ausrichten", "pt": "Alinhar à esquerda", "ru": "По левому краю",
        "zh_CN": "左对齐", "zh_TW": "靠左對齊", "ja": "左揃え", "ko": "왼쪽 맞춤",
        "ar": "محاذاة النص إلى اليمين", "hi": "बाईं ओर संरेखित करें", "nl": "Tekst links uitlijnen",
        "pl": "Wyrównaj do lewej", "tr": "Metni Sola Hizala", "sv": "Vänsterjustera",
        "el": "Στοίχιση αριστερά", "cs": "Zarovnat doleva", "hu": "Balra zárt", "uk": "За лівим краєм", "id": "Rata Kiri"
    },
    "Center": {
        "it": "Allinea al centro", "es": "Centrar", "fr": "Centrer", "de": "Zentrieren", "pt": "Centralizar", "ru": "По центру",
        "zh_CN": "居中", "zh_TW": "置中", "ja": "中央揃え", "ko": "가운데 맞춤", "ar": "توسيط", "hi": "मध्य में",
        "nl": "Centreren", "pl": "Wyśrodkuj", "tr": "Ortala", "sv": "Centrera", "el": "Στοίχιση στο κέντρο", "cs": "Zarovnat na střed",
        "hu": "Középre zárt", "uk": "По центру", "id": "Rata Tengah"
    },
    "Align text right": {
        "it": "Allinea a destra", "es": "Alinear texto a la derecha", "fr": "Aligner le texte à droite",
        "de": "Text rechtsbündig ausrichten", "pt": "Alinhar à direita", "ru": "По правому краю",
        "zh_CN": "右对齐", "zh_TW": "靠右對齊", "ja": "右揃え", "ko": "오른쪽 맞춤",
        "ar": "محاذاة النص إلى اليسار", "hi": "दाईं ओर संरेखित करें", "nl": "Tekst rechts uitlijnen",
        "pl": "Wyrównaj do prawej", "tr": "Metni Sağa Hizala", "sv": "Högerjustera",
        "el": "Στοίχιση δεξιά", "cs": "Zarovnat doprava", "hu": "Jobbra zárt", "uk": "За правим краєм", "id": "Rata Kanan"
    },
    "Justify": {
        "it": "Giustifica", "es": "Justificar", "fr": "Justifier", "de": "Blocksatz", "pt": "Justificar", "ru": "По ширине",
        "zh_CN": "两端对齐", "zh_TW": "左右對齊", "ja": "両端揃え", "ko": "양쪽 맞춤", "ar": "ضبط", "hi": "समान करें",
        "nl": "Uitvullen", "pl": "Wyjustuj", "tr": "İki Yana Yasla", "sv": "Justera", "el": "Πλήρης στοίχιση", "cs": "Do bloku",
        "hu": "Sorkizárt", "uk": "За шириною", "id": "Rata Kanan Kiri"
    },
    "Insert": {
        "it": "Inserisci", "es": "Insertar", "fr": "Insertion", "de": "Einfügen", "pt": "Inserir", "ru": "Вставка",
        "zh_CN": "插入", "zh_TW": "插入", "ja": "挿入", "ko": "삽입", "ar": "إدراج", "hi": "सम्मिलित करें",
        "nl": "Invoegen", "pl": "Wstawianie", "tr": "Ekle", "sv": "Infoga", "el": "Εισαγωγή", "cs": "Vložení",
        "hu": "Beszúrás", "uk": "Вставлення", "id": "Sisipkan"
    },
    "Picture": {
        "it": "Immagine", "es": "Imagen", "fr": "Image", "de": "Bild", "pt": "Imagem", "ru": "Изображение",
        "zh_CN": "图片", "zh_TW": "圖片", "ja": "画像", "ko": "그림", "ar": "صورة", "hi": "चित्र",
        "nl": "Afbeelding", "pl": "Obraz", "tr": "Resim", "sv": "Bild", "el": "Εικόνα", "cs": "Obrázek",
        "hu": "Kép", "uk": "Зображення", "id": "Gambar"
    },
    "Change picture": {
        "it": "Cambia immagine", "es": "Cambiar imagen", "fr": "Changer d'image", "de": "Bild ändern",
        "pt": "Alterar imagem", "ru": "Изменить рисунок", "zh_CN": "更改图片", "zh_TW": "變更圖片",
        "ja": "画像の変更", "ko": "그림 바꾸기", "ar": "تغيير الصورة", "hi": "चित्र बदलें",
        "nl": "Afbeelding wijzigen", "pl": "Zmień obraz", "tr": "Resmi değiştir", "sv": "Ändra bild",
        "el": "Αλλαγή εικόνας", "cs": "Změnit obrázek", "hu": "Kép módosítása", "uk": "Змінити рисунок", "id": "Ubah Gambar"
    },
    "Resize picture": {
        "it": "Ridimensiona immagine", "es": "Cambiar tamaño de imagen", "fr": "Redimensionner l'image", "de": "Bildgröße ändern",
        "pt": "Redimensionar imagem", "ru": "Изменить размер", "zh_CN": "重设图片大小", "zh_TW": "調整圖片大小",
        "ja": "画像のサイズ変更", "ko": "그림 크기 조정", "ar": "تغيير حجم الصورة", "hi": "चित्र का आकार बदलें",
        "nl": "Formaat van afbeelding wijzigen", "pl": "Zmień rozmiar obrazu", "tr": "Resmi Yeniden Boyutlandır", "sv": "Ändra storlek på bild",
        "el": "Αλλαγή μεγέθους εικόνας", "cs": "Změnit velikost obrázku", "hu": "Kép átméretezése", "uk": "Змінити розмір", "id": "Ubah Ukuran Gambar"
    },
    "Paint drawing": {
        "it": "Disegno\ndi Paint", "es": "Dibujo de\nPaint", "fr": "Dessin\nPaint", "de": "Paint-\nZeichnung",
        "pt": "Desenho do\nPaint", "ru": "Рисунок\nPaint", "zh_CN": "绘图\nPaint", "zh_TW": "Paint\n繪圖",
        "ja": "ペイントの\n描画", "ko": "그림판\n그림", "ar": "رسم\nPaint", "hi": "पेंट\nड्राइंग",
        "nl": "Paint-\ntekening", "pl": "Rysunek\nprogramu Paint", "tr": "Paint\nçizimi", "sv": "Paint-\nritning",
        "el": "Ζωγραφική\nPaint", "cs": "Kresba v\nprogramu Paint", "hu": "Paint-\nrajz", "uk": "Рисунок\nPaint", "id": "Lukisan\nPaint"
    },
    "Date and time": {
        "it": "Data\ne ora", "es": "Fecha y\nhora", "fr": "Date et\nheure", "de": "Datum und\nUhrzeit",
        "pt": "Data e\nhora", "ru": "Дата и\nвремя", "zh_CN": "日期\n和时间", "zh_TW": "日期\n與時間",
        "ja": "日付と\n時刻", "ko": "날짜 및\n시간", "ar": "الوقت\nوالتاريخ", "hi": "दिनांक और\nसमय",
        "nl": "Datum en\ntijd", "pl": "Data i\ngodzina", "tr": "Tarih ve\nsaat", "sv": "Datum och\ntid",
        "el": "Ημερομηνία\nκαι ώρα", "cs": "Datum a\nčas", "hu": "Dátum és\nidő", "uk": "Дата й\nчас", "id": "Tanggal dan\nWaktu"
    },
    "Insert object": {
        "it": "Inserisci\noggetto", "es": "Insertar\nobjeto", "fr": "Insérer\nun objet", "de": "Objekt\neinfügen",
        "pt": "Inserir\nobjeto", "ru": "Вставить\nобъект", "zh_CN": "插入\n对象", "zh_TW": "插入\n物件",
        "ja": "オブジェクトの\n挿入", "ko": "개체\n삽입", "ar": "إدراج\nكائن", "hi": "ऑब्जेक्ट\nसम्मिलित करें",
        "nl": "Object\ninvoegen", "pl": "Wstaw\nobiekt", "tr": "Nesne\nEkle", "sv": "Infoga\nobjekt",
        "el": "Εισαγωγή\nαντικειμένου", "cs": "Vložit\nobjekt", "hu": "Objektum\nbeszúrása", "uk": "Вставити\nоб'єкт", "id": "Sisipkan\nObjek"
    },
    "Editing": {
        "it": "Modifica", "es": "Edición", "fr": "Modification", "de": "Bearbeiten", "pt": "Edição", "ru": "Правка",
        "zh_CN": "编辑", "zh_TW": "編輯", "ja": "編集", "ko": "편집", "ar": "تحرير", "hi": "संपादन",
        "nl": "Bewerken", "pl": "Edytowanie", "tr": "Düzenleme", "sv": "Redigering", "el": "Επεξεργασία", "cs": "Úpravy",
        "hu": "Szerkesztés", "uk": "Редагування", "id": "Pengeditan"
    },
    "Find": {
        "it": "Trova", "es": "Buscar", "fr": "Rechercher", "de": "Suchen", "pt": "Localizar", "ru": "Найти",
        "zh_CN": "查找", "zh_TW": "尋找", "ja": "検索", "ko": "찾기", "ar": "بحث", "hi": "खोजें",
        "nl": "Zoeken", "pl": "Znajdź", "tr": "Bul", "sv": "Sök", "el": "Εύρεση", "cs": "Najít",
        "hu": "Keresés", "uk": "Пошук", "id": "Cari"
    },
    "Replace": {
        "it": "Sostituisci", "es": "Reemplazar", "fr": "Remplacer", "de": "Ersetzen", "pt": "Substituir", "ru": "Заменить",
        "zh_CN": "替换", "zh_TW": "取代", "ja": "置換", "ko": "바꾸기", "ar": "استبدال", "hi": "बदलें",
        "nl": "Vervangen", "pl": "Zamień", "tr": "Değiştir", "sv": "Ersätt", "el": "Αντικατάσταση", "cs": "Nahradit",
        "hu": "Csere", "uk": "Замінити", "id": "Ganti"
    },
    "Select all": {
        "it": "Seleziona tutto", "es": "Seleccionar todo", "fr": "Sélectionner tout", "de": "Alles auswählen",
        "pt": "Selecionar tudo", "ru": "Выделить все", "zh_CN": "全选", "zh_TW": "全選",
        "ja": "すべて選択", "ko": "모두 선택", "ar": "تحديد الكل", "hi": "सभी का चयन करें",
        "nl": "Alles selecteren", "pl": "Zaznacz wszystko", "tr": "Tümünü Seç", "sv": "Markera allt",
        "el": "Επιλογή όλων", "cs": "Vybrat vše", "hu": "Mindet kijelöli", "uk": "Виділити все", "id": "Pilih Semua"
    },
    "Zoom": {
        "it": "Zoom", "es": "Zoom", "fr": "Zoom", "de": "Zoom", "pt": "Zoom", "ru": "Масштаб",
        "zh_CN": "缩放", "zh_TW": "縮放", "ja": "ズーム", "ko": "확대/축소", "ar": "تكبير/تصغير", "hi": "ज़ूम",
        "nl": "In-/uitzoomen", "pl": "Powiększenie", "tr": "Yakınlaştır", "sv": "Zooma", "el": "Ζουμ", "cs": "Lupa",
        "hu": "Nagyítás", "uk": "Масштаб", "id": "Zoom"
    },
    "Zoom in": {
        "it": "Ingrandisci", "es": "Acercar", "fr": "Zoom avant", "de": "Vergrößern", "pt": "Mais zoom", "ru": "Увеличить",
        "zh_CN": "放大", "zh_TW": "放大", "ja": "拡大", "ko": "확대", "ar": "تكبير", "hi": "ज़ूम इन",
        "nl": "Inzoomen", "pl": "Powiększ", "tr": "Yakınlaştır", "sv": "Zooma in", "el": "Μεγέθυνση", "cs": "Zvětšit",
        "hu": "Nagyítás", "uk": "Збільшити", "id": "Perbesar"
    },
    "Zoom out": {
        "it": "Riduci", "es": "Alejar", "fr": "Zoom arrière", "de": "Verkleinern", "pt": "Menos zoom", "ru": "Уменьшить",
        "zh_CN": "缩小", "zh_TW": "縮小", "ja": "縮小", "ko": "축소", "ar": "تصغير", "hi": "ज़ूम आउट",
        "nl": "Uitzoomen", "pl": "Pomniejsz", "tr": "Uzaklaştır", "sv": "Zooma ut", "el": "Σμίκρυνση", "cs": "Zmenšit",
        "hu": "Kicsinyítés", "uk": "Зменшити", "id": "Perkecil"
    },
    "100%": {
        "it": "100%", "es": "100%", "fr": "100%", "de": "100%", "pt": "100%", "ru": "100%",
        "zh_CN": "100%", "zh_TW": "100%", "ja": "100%", "ko": "100%", "ar": "100%", "hi": "100%",
        "nl": "100%", "pl": "100%", "tr": "%100", "sv": "100 %", "el": "100%", "cs": "100 %",
        "hu": "100%", "uk": "100%", "id": "100%"
    },
    "Show or hide": {
        "it": "Mostra o nascondi", "es": "Mostrar u ocultar", "fr": "Afficher ou masquer", "de": "Einblenden/Ausblenden",
        "pt": "Mostrar ou ocultar", "ru": "Показать или скрыть", "zh_CN": "显示或隐藏", "zh_TW": "顯示或隱藏",
        "ja": "表示/非表示", "ko": "표시/숨기기", "ar": "إظهار أو إخفاء", "hi": "दिखाएं या छुपाएं",
        "nl": "Weergeven of verbergen", "pl": "Pokazywanie lub ukrywanie", "tr": "Göster veya gizle", "sv": "Visa eller dölj",
        "el": "Εμφάνιση ή απόκρυψη", "cs": "Zobrazit nebo skrýt", "hu": "Megjelenítés vagy elrejtés", "uk": "Відобразити або приховати", "id": "Tampilkan atau Sembunyikan"
    },
    "Ruler": {
        "it": "Righello", "es": "Regla", "fr": "Règle", "de": "Lineal", "pt": "Régua", "ru": "Линейка",
        "zh_CN": "标尺", "zh_TW": "尺規", "ja": "ルーラー", "ko": "눈금자", "ar": "المسطرة", "hi": "रूलर",
        "nl": "Liniaal", "pl": "Linijka", "tr": "Cetvel", "sv": "Linjal", "el": "Χάρακας", "cs": "Pravítko",
        "hu": "Vonalzó", "uk": "Лінійка", "id": "Penggaris"
    },
    "Status bar": {
        "it": "Barra di stato", "es": "Barra de estado", "fr": "Barre d'état", "de": "Statusleiste", "pt": "Barra de status",
        "ru": "Строка состояния", "zh_CN": "状态栏", "zh_TW": "狀態列", "ja": "ステータス バー", "ko": "상태 표시줄",
        "ar": "شريط المعلومات", "hi": "स्थिति पट्टी", "nl": "Statusbalk", "pl": "Pasek stanu", "tr": "Durum çubuğu",
        "sv": "Statusfält", "el": "Γραμμή κατάστασης", "cs": "Stavový řádek", "hu": "Állapotsor", "uk": "Рядок стану", "id": "Bilah Status"
    },
    "Settings": {
        "it": "Impostazioni", "es": "Configuración", "fr": "Paramètres", "de": "Einstellungen", "pt": "Configurações",
        "ru": "Параметры", "zh_CN": "设置", "zh_TW": "設定", "ja": "設定", "ko": "설정", "ar": "الإعدادات", "hi": "सेटिंग्स",
        "nl": "Instellingen", "pl": "Ustawienia", "tr": "Ayarlar", "sv": "Inställningar", "el": "Ρυθμίσεις", "cs": "Nastavení",
        "hu": "Beállítások", "uk": "Параметри", "id": "Pengaturan"
    },
    "Word wrap:": {
        "it": "A capo automatico:", "es": "Ajuste de línea:", "fr": "Retour automatique à la ligne :", "de": "Zeilenumbruch:",
        "pt": "Quebra automática de linha:", "ru": "Перенос по словам:", "zh_CN": "自动换行:", "zh_TW": "自動換行:",
        "ja": "右端で折り返す:", "ko": "자동 줄 바꿈:", "ar": "التفاف النص:", "hi": "शब्द लपेटें:",
        "nl": "Tekstterugloop:", "pl": "Zawijanie wierszy:", "tr": "Sözcük kaydırma:", "sv": "Radbrytning:",
        "el": "Αναδίπλωση λέξεων:", "cs": "Zalamování řádků:", "hu": "Sortörés:", "uk": "Перенесення слів:", "id": "Bungkus Kata:"
    },
    "No wrap": {
        "it": "Nessun a capo", "es": "Sin ajuste", "fr": "Aucun retour automatique", "de": "Kein Umbruch",
        "pt": "Sem quebra automática", "ru": "Без переноса", "zh_CN": "不换行", "zh_TW": "不換行",
        "ja": "折り返さない", "ko": "줄 바꿈 없음", "ar": "بلا التفاف", "hi": "कोई लपेट नहीं",
        "nl": "Geen terugloop", "pl": "Bez zawijania", "tr": "Kaydırma yok", "sv": "Ingen radbrytning",
        "el": "Χωρίς αναδίπλωση", "cs": "Bez zalamování", "hu": "Nincs sortörés", "uk": "Без переносу", "id": "Tanpa Pembungkusan"
    },
    "Wrap to window": {
        "it": "Adatta alla finestra", "es": "Ajustar a la ventana", "fr": "Adapter à la fenêtre", "de": "Am Fenster umbrechen",
        "pt": "Ajustar à janela", "ru": "В границах окна", "zh_CN": "按窗口大小换行", "zh_TW": "依視窗換行",
        "ja": "ウィンドウの幅に合わせる", "ko": "창에 맞게 줄 바꿈", "ar": "التفاف حسب الإطار", "hi": "विंडो पर लपेटें",
        "nl": "Aanpassen aan venster", "pl": "Zawijaj do okna", "tr": "Pencereye kaydır", "sv": "Bryt mot fönster",
        "el": "Αναδίπλωση στο παράθυρο", "cs": "Zalamovat podle okna", "hu": "Igazítás az ablakhoz", "uk": "Переносити за межами вікна", "id": "Bungkus ke Jendela"
    },
    "Wrap to ruler": {
        "it": "Adatta al righello", "es": "Ajustar a la regla", "fr": "Adapter à la règle", "de": "Am Lineal umbrechen",
        "pt": "Ajustar à régua", "ru": "В границах линейки", "zh_CN": "按标尺大小换行", "zh_TW": "依尺規換行",
        "ja": "ルーラーの幅に合わせる", "ko": "눈금자에 맞게 줄 바꿈", "ar": "التفاف حسب المسطرة", "hi": "रूलर पर लपेटें",
        "nl": "Aanpassen aan liniaal", "pl": "Zawijaj do linijki", "tr": "Celvele göre kaydır", "sv": "Bryt mot linjal",
        "el": "Αναδίπλωση στο χάρακα", "cs": "Zalamovat podle pravítka", "hu": "Igazítás a vonalzóhoz", "uk": "Переносити за лінійкою", "id": "Bungkus ke Penggaris"
    },
    "Measurement units:": {
        "it": "Unità di misura:", "es": "Unidades de medida:", "fr": "Unités de mesure :", "de": "Maßeinheiten:",
        "pt": "Unidades de medida:", "ru": "Единицы измерения:", "zh_CN": "度量单位:", "zh_TW": "度量單位:",
        "ja": "測定単位:", "ko": "단위:", "ar": "وحدات القياس:", "hi": "माप इकाइयां:",
        "nl": "Maateenheden:", "pl": "Jednostki miary:", "tr": "Ölçü birimleri:", "sv": "Måttenheter:",
        "el": "Μονάδες μέτρησης:", "cs": "Měrné jednotky:", "hu": "Mértékegység:", "uk": "Одиниці вимірювання:", "id": "Satuan Ukuran:"
    },
    "Inches": {
        "it": "Pollici", "es": "Pulgadas", "fr": "Pouces", "de": "Zoll", "pt": "Polegadas", "ru": "Дюймы",
        "zh_CN": "英寸", "zh_TW": "英吋", "ja": "インチ", "ko": "인치", "ar": "بوصة", "hi": "इंच",
        "nl": "Inches", "pl": "Cale", "tr": "İnç", "sv": "Tum", "el": "Ίντσες", "cs": "Palce",
        "hu": "Hüvelyk", "uk": "Дюйми", "id": "Inci"
    },
    "Centimeters": {
        "it": "Centimetri", "es": "Centímetros", "fr": "Centimètres", "de": "Zentimeter", "pt": "Centímetros", "ru": "Сантиметры",
        "zh_CN": "厘米", "zh_TW": "公分", "ja": "センチメートル", "ko": "센티미터", "ar": "سنتيمترات", "hi": "सेंटीमीटर",
        "nl": "Centimeters", "pl": "Centymetry", "tr": "Santimetre", "sv": "Centimeter", "el": "Εκατοστά", "cs": "Centimetry",
        "hu": "Centiméter", "uk": "Сантиметри", "id": "Sentimeter"
    },
    "Points": {
        "it": "Punti", "es": "Puntos", "fr": "Points", "de": "Punkte", "pt": "Pontos", "ru": "Пункты",
        "zh_CN": "磅", "zh_TW": "點", "ja": "ポイント", "ko": "포인트", "ar": "نقاط", "hi": "अंक",
        "nl": "Punten", "pl": "Punkty", "tr": "Nokta", "sv": "Punkter", "el": "Στιγμές", "cs": "Body",
        "hu": "Pont", "uk": "Пункти", "id": "Poin"
    },
    "Picas": {
        "it": "Pica", "es": "Picas", "fr": "Picas", "de": "Pica", "pt": "Picas", "ru": "Пики",
        "zh_CN": "皮卡", "zh_TW": "十二點活字", "ja": "パイカ", "ko": "파이카", "ar": "بيكاس", "hi": "पिकास",
        "nl": "Pica's", "pl": "Cyceros", "tr": "Pika", "sv": "Pica", "el": "Pica", "cs": "Pica",
        "hu": "Pica", "uk": "Піка", "id": "Pica"
    },
    "Language:": {
        "it": "Lingua:", "es": "Idioma:", "fr": "Langue :", "de": "Sprache:", "pt": "Idioma:", "ru": "Язык:",
        "zh_CN": "语言:", "zh_TW": "語言:", "ja": "言語:", "ko": "언어:", "ar": "اللغة:", "hi": "भाषा:",
        "nl": "Taal:", "pl": "Język:", "tr": "Dil:", "sv": "Språk:", "el": "Γλώσσα:", "cs": "Jazyk:",
        "hu": "Nyelv:", "uk": "Мова:", "id": "Bahasa:"
    },
    "Document": {
        "it": "Documento", "es": "Documento", "fr": "Document", "de": "Dokument", "pt": "Documento", "ru": "Документ",
        "zh_CN": "文档", "zh_TW": "文件", "ja": "ドキュメント", "ko": "문서", "ar": "مستند", "hi": "दस्तावेज़",
        "nl": "Document", "pl": "Dokument", "tr": "Belge", "sv": "Dokument", "el": "Έγγραφο", "cs": "Dokument",
        "hu": "Dokumentum", "uk": "Документ", "id": "Dokumen"
    },
    "Ready": {
        "it": "Pronto", "es": "Listo", "fr": "Prêt", "de": "Bereit", "pt": "Pronto", "ru": "Готово",
        "zh_CN": "就绪", "zh_TW": "就緒", "ja": "準備完了", "ko": "준비", "ar": "جاهز", "hi": "तैयार",
        "nl": "Gereed", "pl": "Gotowe", "tr": "Hazır", "sv": "Klar", "el": "Έτοιμο", "cs": "Připraven",
        "hu": "Kész", "uk": "Готово", "id": "Siap"
    },
    "Do you want to save changes to \"%1\"?": {
        "it": "Salvare le modifiche a \"%1\"?", "es": "¿Desea guardar los cambios en \"%1\"?",
        "fr": "Voulez-vous enregistrer les modifications apportées à « %1 » ?", "de": "Möchten Sie Änderungen an \"%1\" speichern?",
        "pt": "Deseja salvar as alterações em \"%1\"?", "ru": "Сохранить изменения в \"%1\"?",
        "zh_CN": "是否保存对 \"%1\" 的更改？", "zh_TW": "您要儲存對 \"%1\" 的變更嗎？",
        "ja": "\"%1\" への変更を保存しますか?", "ko": "\"%1\"의 변경 내용을 저장하시겠습니까?",
        "ar": "هل تريد حفظ التغييرات في \"%1\"؟", "hi": "क्या आप \"%1\" में परिवर्तनों को सहेजना चाहते हैं?",
        "nl": "Wilt u wijzigingen in \"%1\" opslaan?", "pl": "Czy chcesz zapisać zmiany w pliku „%1”?",
        "tr": "\"%1\" dosyasındaki değişiklikleri kaydetmek istiyor musunuz?", "sv": "Vill du spara ändringarna i \"%1\"?",
        "el": "Θέλετε να αποθηκεύσετε τις αλλαγές στο \"%1\";", "cs": "Chcete uložit změny v souboru \"%1\"?",
        "hu": "Menti a(z) „%1” változtatásait?", "uk": "Зберегти зміни у \"%1\"?", "id": "Apakah Anda ingin menyimpan perubahan ke \"%1\"?"
    }
}

def generate_ts_files():
    out_dir = "/home/leo/src/openwordpad/resources/translations"
    os.makedirs(out_dir, exist_ok=True)
    
    contexts = ["RibbonBar", "MainWindow", "OpenWordPad::MainWindow", "OpenWordPad::AboutDialog", "AboutDialog", "FindReplaceDialog", "ParagraphDialog", "PageSetupDialog", "DateTimeDialog", "ImageResizeDialog", "PaintDrawingDialog"]

    for lang_code, lang_name in LANGUAGES.items():
        if lang_code == "en":
            # Base language
            continue
            
        root = ET.Element("TS", version="2.1", language=lang_code)
        
        for ctx_name in contexts:
            ctx_el = ET.SubElement(root, "context")
            name_el = ET.SubElement(ctx_el, "name")
            name_el.text = ctx_name
            
            for source, trans_map in TRANSLATIONS.items():
                msg_el = ET.SubElement(ctx_el, "message")
                src_el = ET.SubElement(msg_el, "source")
                src_el.text = source
                
                trans_text = trans_map.get(lang_code, source)
                trans_el = ET.SubElement(msg_el, "translation")
                trans_el.text = trans_text
                
        tree = ET.ElementTree(root)
        ts_path = os.path.join(out_dir, f"openwordpad_{lang_code}.ts")
        qm_path = os.path.join(out_dir, f"openwordpad_{lang_code}.qm")
        
        tree.write(ts_path, encoding="utf-8", xml_declaration=True)
        
        # Compile to .qm using lrelease6
        cmd = ["/usr/bin/lrelease6", ts_path, "-qm", qm_path]
        subprocess.run(cmd, check=True)
        print(f"Generated {qm_path} ({lang_name})")

if __name__ == "__main__":
    generate_ts_files()
