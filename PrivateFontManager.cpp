/**
 * @file PrivateFontManager.cpp
 * @brief
 * プライベートフォント管理クラス（ Multibyte / Unicode ）
 * @author イネマル
 * @date 2015/5/2	(開始)
 * @date 2015/5/28	(最終更新)
 */

/**
 * 2015/5/28 v1.00 -> v1.01
 * ・マルチ バイト文字セットに対応
*/

//********************************************************************
//! インクルード
//********************************************************************
#include "PrivateFontManager.h"

//********************************************************************
//! 静的オブジェクト
//********************************************************************
GDIPlusBoot		FontNameGetter::m_GDIPlus;

//********************************************************************
//! 関数定義
//********************************************************************

/**
 * @fn
 * エラー用デバッグメッセージボックス
 * @breaf エラー用メッセージボックス
 * @param (tstring funcname) エラーが発生した関数名
 * @param (LPTSTR format) エラーの内容（書式形式可能）
 * @return なし
 */
void ErrorMsgBox(tstring funcname, LPTSTR format, ...) {
	TCHAR str[1024];
	va_list list;
	va_start(list, format);
	_vstprintf_s(str, 1024, format, list);
	va_end(list);
	funcname = funcname + _T('\n') + str;
	MessageBox(GetForegroundWindow(), funcname.c_str(), _T("エラー"), MB_OK | MB_ICONERROR);
}

//********************************************************************
//! GDI+の起動管理クラス
//! GDIPlusBoot
//********************************************************************

/**
 * @fn
 * コンストラクタ
 * @breaf コンストラクタ
 * @param (void)
 * @detail
 * GDI+の起動
 */
GDIPlusBoot::GDIPlusBoot() {
	GdiplusStartup(&m_gdiToken, &m_gdiSI, NULL);
}

/**
 * @fn
 * デストラクタ
 * @breaf デストラクタ
 * @param (void)
 * @detail
 * GDI+の終了
 */
GDIPlusBoot::~GDIPlusBoot() {
	GdiplusShutdown(m_gdiToken);
}

//********************************************************************
//! フォント名取得クラス
//! FontNameGetter
//********************************************************************

/**
 * @fn
 * コンストラクタ
 * @breaf コンストラクタ
 * @param (void)
 * @detail
 * ロケール設定を日本に変更
 */
FontNameGetter::FontNameGetter() {
	// 日本語フォント名対応処理
	setlocale(LC_CTYPE, "jpn");
}

/**
 * @fn
 * デストラクタ
 * @breaf デストラクタ
 * @param (void)
 */
FontNameGetter::~FontNameGetter() {
}

/**
 * @fn
 * FontNameGetterクラスを使用するための初期化を行う
 * @breaf FontNameGetterクラスの初期化
 * @param (LPCTSTR lpcFilePath) フォントの場所を指す文字列パス
 * @return 
 * 成功：ファイルから取得されたフォント名の文字列ポインタ
 * ただし、複数ある場合は、リストの最後で取得された文字列ポインタ
 * 失敗：NULL
 * @detail
 * privateFontCollectionは、フォント名を取得する為のみで使用するクラスのため
 * フィールドとして扱わない
 */
LPCTSTR FontNameGetter::GetFontNameFromFile(LPCTSTR lpcFilePath) {
	int						count = 0;
	int						found = 0;
	PrivateFontCollection	privateFontCollection;
	WCHAR					familyName[LF_FACESIZE] = {};
	FontFamily*				pFontFamily = NULL;
	// フォントコレクションにフォントを追加する
#ifdef _UNICODE
	switch( privateFontCollection.AddFontFile(lpcFilePath) ) {
	case 0:		// Gdiplus::Status::Ok
		break;

	case 10:	// Gdiplus::Status::FileNotFound
		ErrorMsgBox(__FUNCTIONW__, L"ファイルが見つかりませんでした。");
		return NULL;

	default:
		ErrorMsgBox(__FUNCTIONW__, L"エラーが発生しました");
		return NULL;
	}
#else
	int FilePathlen = strlen(lpcFilePath);
	wchar_t* tmpfamilyName = new wchar_t[ FilePathlen + 1 ];
	if( mbstowcs_s(NULL, tmpfamilyName, FilePathlen + 1, lpcFilePath, _TRUNCATE) == -1 ) {
		ErrorMsgBox(__FUNCTION__, "文字セットの変換に失敗しました。");
		delete tmpfamilyName;
		return NULL;
	}
	switch( privateFontCollection.AddFontFile(tmpfamilyName) ) {
	case 0:		// Gdiplus::Status::Ok
		break;

	case 10:	// Gdiplus::Status::FileNotFound
		ErrorMsgBox(__FUNCTION__, "ファイルが見つかりませんでした。");
		return NULL;

	default:
		ErrorMsgBox(__FUNCTION__, "エラーが発生しました");
		return NULL;
	}
	delete tmpfamilyName;
#endif
	count = privateFontCollection.GetFamilyCount();
	pFontFamily = new FontFamily[count];
	privateFontCollection.GetFamilies(count, pFontFamily, &found);
	m_FamilyName.resize(count);
	// フォント名のリストを作成
	for( int i = 0; i < count; i++ ){
#ifdef _UNICODE
		pFontFamily[i].GetFamilyName(familyName);
		m_FamilyName[i] = familyName;
#else
		pFontFamily[i].GetFamilyName(familyName);
		int familyNamelen = wcslen(familyName);
		char* tmpfamilyName = new char[ familyNamelen*2 + 1 ];
		if( wcstombs_s( NULL, tmpfamilyName, familyNamelen*2+1, familyName, _TRUNCATE) == -1 ) {
			ErrorMsgBox(__FUNCTION__, "エラーが発生しました");
			delete tmpfamilyName;
			return NULL;
		}
		m_FamilyName[i] = tstring( tmpfamilyName );
		delete tmpfamilyName;
#endif
	}
	delete[] pFontFamily;
	return m_FamilyName[0].c_str();
}

/**
 * @fn
 * GetFontNameFromFileによって取得されたフォント名のリストにアクセスする
 * @breaf フォント名のリストへのアクセサ
 * @param (UINT uiIdx) リストにアクセスするための添え字
 * @return 
 * 成功：アクセスされたフォント名の文字列ポインタ
 * 失敗：NULL
 * @sa GetFontNameFromFile
 * @detail
 * inline関数にした方が、高速化を図れるかも知れない
 */
LPCTSTR FontNameGetter::GetFontNameFromList(UINT uiIdx) {
	return ( uiIdx >= m_FamilyName.size() ? NULL : m_FamilyName[uiIdx].c_str() );
}

/**
 * @fn
 * GetFontNameFromFileによって取得されたフォント名のリストにアクセスする
 * @breaf フォント名のリストへのアクセサ
 * @param (UINT uiIdx) リストにアクセスするための添え字
 * @return 
 * 成功：アクセスされたフォント名の文字列ポインタ(書き換え可能)
 * 失敗：NULL
 * @sa GetFontNameFromFile
 * @detail
 * 設計上、フィールドを直接書き換えが可能になってしまうので非推奨メソッド
 * inline関数にした方が、高速化を図れるかも知れない
 */
LPTSTR FontNameGetter::GetFontNameFromList_tstr(UINT uiIdx) {
	return ( uiIdx >= m_FamilyName.size() ? NULL : &m_FamilyName[uiIdx][0] );
}

/**
 * @fn
 * GetFontNameFromFileによって取得されたフォント名のリストから文字列をコピーする
 * @breaf フォント名のリストからフォント名をコピーする
 * @param (OUT LPTSTR lpFontName) コピー先への、文字列ポインタ
 * @param (UINT uiIdx) リストにアクセスするための添え字
 * @return コピー処理が終了した引数のlpFontName
 * @sa GetFontNameFromFile
 * @sa GetFontNameFromList_tstr
 * @detail
 * GetFontNameFromList_tstrの問題回避版
 */
LPTSTR FontNameGetter::CopyFontNameFromList(OUT LPTSTR lpFontName,UINT uiIdx) {
	for( UINT i = 0; i < m_FamilyName[0].size(); i++ ) {
		lpFontName[i] = m_FamilyName[uiIdx][i];
	}
	return lpFontName;
}

/**
 * @fn
 * GetFontNameFromFileによって取得されたフォント名の数を取得する
 * @breaf フォント名のリストに取得されたフォントの数を取得
 * @param (void) 
 * @return フォント名のリストに取得されたフォントの数
 * @sa GetFontNameFromFile
 * @detail
 * inline関数にした方が、高速化を図れるかも知れない
 */
UINT FontNameGetter::GetFontListCount(void) {
	return m_FamilyName.size();
}


//********************************************************************
//! プライベートフォント管理クラス
//! PrivateFontManager
//********************************************************************

/**
 * @fn
 * コンストラクタ
 * @breaf コンストラクタ
 * @param (void)
 * @detail
 * フィールドの初期化
 */
PrivateFontManager::PrivateFontManager() {
	m_uiHandle = 0;
	m_FontCount = 0;
}

/**
 * @fn
 * デストラクタ
 * @breaf デストラクタ
 * @param (void)
 * @detail
 * インストールされたフォントの解放
 */
PrivateFontManager::~PrivateFontManager() {
	for( UINT i = 0; i < m_FilePathList.size(); i++ ) {
		if ( m_FontCount > 0 && RemoveFontResourceEx(m_FilePathList[i].c_str(), FR_PRIVATE, NULL) == 0 ) {
#ifdef _UNICODE
		ErrorMsgBox(__FUNCTIONW__, L"フォントの解放に失敗しました。");
#else
		ErrorMsgBox(__FUNCTION__, "フォントの解放に失敗しました。");
#endif
		}
	}
}

/**
 * @fn
 * プライベートフォントをインストールし使用可能にする
 * @breaf プライベートフォントのインストール
 * @param (LPCTSTR lpcFilePath) フォントの場所を指す文字列パス
 * @return
 * 成功：インストールしたフォントへのアクセス用ハンドル
 * 失敗：(INT)-1
 * @detail
 * 解放用のファイルパスをリストに追加する処理
 * フォントのインストールが行われる
 */
INT PrivateFontManager::InstallPrivateFont(LPCTSTR lpcFilePath) {
	m_FilePathList.push_back(lpcFilePath);
	m_FontCount = AddFontResourceEx(lpcFilePath, FR_PRIVATE, NULL);
	if ( m_FontCount == 0 ) {
#ifdef _UNICODE
		ErrorMsgBox(__FUNCTIONW__, _T("フォントの取得に失敗しました。"));
#else
		ErrorMsgBox(__FUNCTION__,"フォントの取得に失敗しました。");
#endif
		return -1;
	}
	FontNameGetter pushinstance;
	if( pushinstance.GetFontNameFromFile(lpcFilePath) == NULL ) {
		return -1;
	}
	m_FontName.push_back(pushinstance);
	return m_uiHandle++;
}

/**
 * @fn
 * 取得されたフォント名のリストにアクセスする
 * @breaf フォント名のリストへのアクセサ
 * @param (UINT uiIdx) リストにアクセスするための添え字
 * @return 
 * 成功：アクセスされたフォント名の文字列ポインタ
 * 失敗：NULL
 * @detail
 * 内包しているFontNameGetterのアクセスラッパ
 * inline関数にした方が、高速化を図れるかも知れない
 */
LPCTSTR	PrivateFontManager::GetFontNameFromList(UINT uiHandle, UINT uiIdx) {
	return ( uiHandle >= m_FontName.size() ? NULL : m_FontName[uiHandle].GetFontNameFromList(uiIdx) );
}

/**
 * @fn
 * 取得されたフォント名の数を取得する
 * @breaf フォント名のリストに取得されたフォントの数へのアクセサ
 * @param (void) 
 * @return フォント名のリストに取得されたフォントの数
 * @detail
 * 内包しているFontNameGetterのアクセスラッパ
 * inline関数にした方が、高速化を図れるかも知れない
 */
UINT PrivateFontManager::GetFontListCount(UINT uiHandle) {
	return ( uiHandle >= m_FontName.size() ? NULL : m_FontName[uiHandle].GetFontListCount() );
}
