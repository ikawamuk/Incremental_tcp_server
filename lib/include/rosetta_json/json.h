/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 22:08:43 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/04 18:19:56 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JSON_H
# define JSON_H

# include <stdbool.h>
# include <stddef.h>

# define JSON_NESTING_LIMIT 10000

typedef enum e_json_type
{
	JSON_Null,
	JSON_True,
	JSON_False,
	JSON_Number,
	JSON_String,
	JSON_Array,
	JSON_Object
}	t_json_type;

typedef enum e_json_error_code
{
	NO_ERROR,
	FAILED_TO_MEMORY_ALLOCATION,
	INVALID_TOKEN,
	NESTING_IS_TOO_DEEP,
	TRAILING_GARBAGE
}	t_error_code;

typedef struct s_json_array
{
	struct s_json_array	*prev;
	struct s_json_array	*next;
	struct s_json		*element;
}	t_json_array;

typedef struct s_json_object
{
	struct s_json_object	*prev;
	struct s_json_object	*next;
	char					*key;
	struct s_json			*value;
}	t_json_object;

typedef struct s_json
{
	t_json_type		type;
	union u_json_data
	{
		bool			bool_data;
		double			number_data;
		char			*string_data;
		t_json_array	*array_data;
		t_json_object	*object_data;
	}	_;
}	t_json;

/**
 * @brief JSON文字列を解析し、t_jsonオブジェクト構造体を生成します。
 * @param json_text 解析対象のJSON文字列。NULL終端文字列でなければいけない。
 * @return 正常時は生成された t_json へのポインタ。失敗時は NULL
 * @note 戻り値のメモリは不要になったら json_delete() で解放してください。解放の責任は呼び出し元にあります
 */
t_json	*json_parse(const char *json_text);

/**
 * @brief t_jsonオブジェクトとそれに含まれるすべての要素を再帰的に削除（メモリ解放）します。
 * @param item 解放する t_json オブジェクトへのポインタ
 * @return なし
 * @note NULLを渡した場合は何も行われません。
 */
void	json_delete(t_json *item);

/**
 * @brief t_jsonオブジェクトを整形済み（インデント・改行あり）のJSON文字列に変換します。
 * @param item 変換対象の t_json オブジェクト
 * @return 生成されたJSON文字列へのポインタ（失敗時は NULL）
 * @note 戻り値の文字列バッファは呼び出し元で free() などによる解放が必要です。
 */
char	*json_generate(const t_json *item);

/**
 * @brief t_jsonオブジェクトを非整形（改行・空白なしの軽量形式）のJSON文字列に変換します。
 * @param item 変換対象の t_json オブジェクト
 * @return 生成された圧縮JSON文字列へのポインタ（失敗時は NULL）
 * @note 戻り値の文字列バッファは呼び出し元で free() などによる解放が必要です。ネットワーク転送等に適しています。
 */
char	*json_generate_unformatted(const t_json *item);

/**
 * @brief 最後に発生したJSON処理エラーのエラーコードを取得します。
 * @param _ なし
 * @return エラーコードを表す整数値（エラーがない場合は 0 ）
 * @note スレッドローカルなエラー状態を返す設計が推奨されます。
 */
int		json_get_error_code(void);

/**
 * @brief エラーコードに対応する人間が読める形式のエラーメッセージ文字列を取得します。
 * @param error_code json_get_error_code() で取得したエラーコード
 * @return エラー内容を示す文字列へのポインタ
 * @note 返される文字列は静的領域のものとし、呼び出し元で解放しないでください。
 */
char	*json_get_error_message(int error_code);

/**
 * @brief JSONパース失敗時、エラーが発生したバイトオフセット（位置）を取得します。
 * @param _ なし
 * @return 0始まりのエラー発生位置。エラーがない場合は (size_t)-1
 * @note 先頭でエラーが起きた場合は 0 を返します。
 */
size_t	json_get_error_position(void);

/**
 * @brief JSON要素が null であるか判定します。
 * @param item 判定対象の JSON要素（NULL 許容）
 * @return item が null の場合は true、それ以外および item が NULL の場合は false
 * @note 安全のため、item が NULL ポインタであっても呼び出し可能です。
 */
bool	json_is_null(t_json const *const item);

/**
 * @brief JSON要素が true であるか判定します。
 * @param item 判定対象の JSON要素（NULL 許容）
 * @return item が true の場合は true、それ以外および item が NULL の場合は false
 * @note 安全のため、item が NULL ポインタであっても呼び出し可能です。
 */
bool	json_is_true(t_json const *const item);

/**
 * @brief JSON要素が false であるか判定します。
 * @param item 判定対象の JSON要素（NULL 許容）
 * @return item が false の場合は true、それ以外および item が NULL の場合は false
 * @note 安全のため、item が NULL ポインタであっても呼び出し可能です。
 */
bool	json_is_false(t_json const *const item);

/**
 * @brief JSON要素が数値（Number）であるか判定します。
 * @param item 判定対象の JSON要素（NULL 許容）
 * @return item が数値の場合は true、それ以外および item が NULL の場合は false
 * @note 安全のため、item が NULL ポインタであっても呼び出し可能です。
 */
bool	json_is_number(t_json const *const item);

/**
 * @brief 数値の JSON要素から double 値を取得します。
 * @param number 値を取得したい数値型の JSON要素
 * @return 格納されている double 値。number が NULL または数値型でない場合は NaN
 * @note 事前条件として json_is_number(number) が true であることを推奨します。
 */
double	json_get_number(t_json const *const number);

/**
 * @brief JSON要素が文字列（String）であるか判定します。
 * @param item 判定対象の JSON要素（NULL 許容）
 * @return item が文字列の場合は true、それ以外および item が NULL の場合は false
 * @note 安全のため、item が NULL ポインタであっても呼び出し可能です。
 */
bool	json_is_string(t_json const *const item);

/**
 * @brief 文字列の JSON要素から文字列ポインタを取得します。
 * @param string 値を取得したい文字列型の JSON要素
 * @return 格納されている文字列（char *）のポインタ。string が NULL または文字列型でない場合は NULL
 * @note 返されるポインタは内部保持データのため、呼び出し側で free しないでください。
 */
char	*json_get_string(t_json const *const string);

/**
 * @brief JSON要素が配列（Array）であるか判定します。
 * @param item 判定対象の JSON要素（NULL 許容）
 * @return item が配列の場合は true、それ以外および item が NULL の場合は false
 * @note 安全のため、item が NULL ポインタであっても呼び出し可能です。
 */
bool	json_is_array(t_json const *const item);

/**
 * @brief 配列の要素数を取得します。
 * @param array 要素数を取得したい配列型の JSON要素
 * @return 配列に含まれる要素数。array が NULL または配列型でない場合は 0
 * @note array が空の配列（[]）の場合も 0 を返します。
 */
size_t	json_get_array_size(t_json const *const array);

/**
 * @brief 配列から指定したインデックスの JSON要素を取得します。
 * @param array 参照対象の配列型の JSON要素
 * @param index 取得したい要素の 0 始まりのインデックス
 * @return 指定したインデックスに位置する JSON要素へのポインタ。インデックスが範囲外、
 * 			または array が NULL / 非配列の場合は NULL
 * @note インデックスの有効範囲は 0 <= index < json_get_array_size(array) です。
 */
t_json	*json_get_array_element(t_json const *const array, size_t index);

/**
 * @brief JSON要素がオブジェクト（Object）であるか判定します。
 * @param item 判定対象の JSON要素（NULL 許容）
 * @return item がオブジェクトの場合は true、それ以外および item が NULL の場合は false
 * @note 安全のため、item が NULL ポインタであっても呼び出し可能です。
 */
bool	json_is_object(t_json const *const item);

/**
 * @brief オブジェクト内に指定したキーが存在するか確認します。
 * @param item object 検索対象のオブジェクト型の JSON要素
 * @param  key 検索するキー文字列
 * @return 指定したキーが存在する場合は true、存在しない場合や引数が不適切な場合は false
 * @note 安全のため、object や key が NULL であっても呼び出し可能です。
 */
bool	json_has_object_item(t_json const *const object, const char *key);

/**
 * @brief オブジェクトから指定したキーに対応する JSON要素を取得します。
 * @param item object 参照対象のオブジェクト型の JSON要素
 * @param  key 取得したいフィールドのキー文字列
 * @return キーに対応する JSON要素へのポインタ。キーが存在しない、または object が NULL / 非オブジェクトの場合は NULL
 * @note キーの比較は大文字・小文字を厳密に区別します。
 */
t_json	*json_get_object_item(t_json const *const object, const char *key);

#endif
