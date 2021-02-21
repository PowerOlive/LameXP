///////////////////////////////////////////////////////////////////////////////
// LameXP - Audio Encoder Front-End
// Copyright (C) 2004-2021 LoRd_MuldeR <MuldeR2@GMX.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU GENERAL PUBLIC LICENSE as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version; always including the non-optional
// LAMEXP GNU GENERAL PUBLIC LICENSE ADDENDUM. See "License.txt" file!
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// http://www.gnu.org/licenses/gpl-2.0.txt
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#ifndef LAMEXP_INC_TOOLS
#error Please do *not* include TOOLS.H directly!
#endif

////////////////////////////////////////////////////////////
// CPU FLags
////////////////////////////////////////////////////////////

/* CPU_TYPE_<ARCH>_<TYPE> */
#define CPU_TYPE_X86_GEN 0x00000001UL //x86, generic
#define CPU_TYPE_X86_SSE 0x00000002UL //x86, with SSE and SSE2 support - Intel only!
#define CPU_TYPE_X86_AVX 0x00000004UL //x86, with AVX support
#define CPU_TYPE_X64_GEN 0x00000008UL //x64, generic
#define CPU_TYPE_X64_SSE 0x00000010UL //x64, with SSE and SSE2 support - Intel only!
#define CPU_TYPE_X64_AVX 0x00000020UL //x64, with AVX support

/* combined CPU types */
#define CPU_TYPE_X86_NVX (CPU_TYPE_X86_GEN|CPU_TYPE_X86_SSE)
#define CPU_TYPE_X86_SSX (CPU_TYPE_X86_SSE|CPU_TYPE_X86_AVX)
#define CPU_TYPE_X86_ALL (CPU_TYPE_X86_GEN|CPU_TYPE_X86_SSX)
#define CPU_TYPE_X64_NVX (CPU_TYPE_X64_GEN|CPU_TYPE_X64_SSE)
#define CPU_TYPE_X64_SSX (CPU_TYPE_X64_SSE|CPU_TYPE_X64_AVX)
#define CPU_TYPE_X64_ALL (CPU_TYPE_X64_GEN|CPU_TYPE_X64_SSX)
#define CPU_TYPE_ALL_GEN (CPU_TYPE_X86_GEN|CPU_TYPE_X64_GEN)
#define CPU_TYPE_ALL_SSE (CPU_TYPE_X86_SSE|CPU_TYPE_X64_SSE)
#define CPU_TYPE_ALL_SSX (CPU_TYPE_X86_SSX|CPU_TYPE_X64_SSX)
#define CPU_TYPE_ALL_AVX (CPU_TYPE_X86_AVX|CPU_TYPE_X64_AVX)
#define CPU_TYPE_ALL_ALL (CPU_TYPE_X86_ALL|CPU_TYPE_X64_ALL)

////////////////////////////////////////////////////////////
// TOOLS
////////////////////////////////////////////////////////////

static const struct
{
	const char *pcHash;
	const unsigned int uiCpuType;
	const char *pcName;
	const unsigned int uiVersion;
	const char *pcVersTag;
}
g_lamexp_tools[] =
{
	{"4870d7abbc3995dc5ca82147ee33f28c3ee7dc1986fbb0131912effe10c1a209d3f983c5dffd963c31707a5ce39611b4", CPU_TYPE_X86_GEN, "aften.i686.exe", 8, ""},
	{"fc4e38b11a0f52b68cca79aa2d71c02180b70eb0e592923dee4f0ccf766f1006642369b2178f6a61d1c2506446cc442d", CPU_TYPE_X86_SSX, "aften.sse2.exe", 8, ""},
	{"b44b1c0e1a8f4633c9bc2e43d0a83ea5edc890e1cccc0054c812d0e64170e5292dc1dfd7430da61b73150831aa16e9c4", CPU_TYPE_X64_ALL, "aften.x64.exe",  8, ""},
	{"4558728999a05f12fe88566e8308cba0ada200814c2a1bfe2507f49faf8f3994b0d52a829148f5c6321d24faa2718439", CPU_TYPE_ALL_ALL, "avs2wav.exe", 13, ""},
	{"a86150132522f06fd4cdd3b00bc95c1c76520a8c754e50d483f9a9bb00893064a86d3c880b37cb0217a6abddfa46a765", CPU_TYPE_ALL_ALL, "curl.exe", 7750, ""},
	{"07834b1b8ecac2f3db79ad048005eca3a284cb759e15d399eb1a560a403122e20db5f08e60319759b8463aeef2410d02", CPU_TYPE_ALL_ALL, "dcaenc.exe", 20120419, ""},
	{"5c4a5cdd708b5857bcb064558be81a2dfb16401e796b296f6eee7c63042acbeae12e1e2f1f3d0fd096eaf73201b54e10", CPU_TYPE_ALL_GEN, "faad.i686.exe", 270, "" },
	{"72447794cf411e1e4ce71facf5f60023f001d203894cf40185a0ee13e144e93d72ac99f2ed30a9168450ce5bf882f99f", CPU_TYPE_ALL_SSX, "faad.sse2.exe", 270, "" },
	{"488c59c426ad386ea1e025dcaa8031d07cdc3a07b6c88d965daddb8360c80d5d35c745f245ca805cc78509ad1f05317a", CPU_TYPE_X86_GEN, "flac.x86-i686.exe", 132, ""},
	{"a51ab90f0a4748d1d1fd738ef168b0ad0a65be5af726fdfecc94392d74bd87185f9340b9643ce59352322c3477e7b9e6", CPU_TYPE_X86_SSX, "flac.x86-sse2.exe", 132, ""},
	{"dfe577ab7265c4f62c097024ace4a45d1b35ee6a458c68e830a7e63b3020198f161e0deabe2605820f49575bd4724d53", CPU_TYPE_X64_NVX, "flac.x64-sse2.exe", 132, ""},
	{"84554229476537d06f2aafc0ca8153eb971b7f44c67f0ca9474e557b795f0cd055b9dd9af6a72d0dd37c45d952b0b479", CPU_TYPE_X64_AVX, "flac.x64-avx.exe",  132, ""},
	{"d2cff6f3ca2e5147d4ac76b059052fbcbbabf6c3163e188edef80002ff30283aef2fee43fdc905bd71f1a0a59317ebec", CPU_TYPE_ALL_ALL, "gpgv.exe", 1423, ""},
	{"c66992384b6388a7b68f2659624c150b33ff1cacb0c28c0ab2a3249eca4c31e204953f325403f78c077d1735b275ac12", CPU_TYPE_ALL_ALL, "keyring.gpg", UINT_MAX, ""},
	{"657320078375a5392e2f9351f2b375b5b3fd9c7ab5e00d0d1f00e58af2405f9f9278078b17959f1d547d4c1b816b0b67", CPU_TYPE_X86_GEN, "lame.x86-i686.exe", 31001, "SVN"},
	{"d26a917f5cd1b29bb10810d85e4f979e3fd58d9c4957990badd35a57bc6936b02d78c0096094e7906c8288f66b79bb8b", CPU_TYPE_X86_SSX, "lame.x86-sse2.exe", 31001, "SVN"},
	{"b76d98075dac1a18675d837aea4051a9a71406af4636a41f0a8923beaf0d299de3ad51a6d80275a878849a63de16e90a", CPU_TYPE_X64_NVX, "lame.x64-sse2.exe", 31001, "SVN" },
	{"31c4374e174bbf25b4429eabf22f570a841ce63796ce8321ea02af9620ce599cf9f92276224acc8f9375cdcaae846c8c", CPU_TYPE_X64_AVX, "lame.x64-avx.exe",  31001, "SVN" },
	{"c8367a4f5baf64cac42aa8f68f5c64360b1c79edd91cc6cb67f3f4d0fdfc85a51af82c8930c35cc14944bfde0abafde3", CPU_TYPE_X86_GEN, "mac.x86-i686.exe", 612, ""},
	{"0dbaee5e265caaeb97534eee0a3135657d2a3456b174f5d66c723e157b33c0df6827abcb37c1035b9c82ca359a39db3a", CPU_TYPE_X86_SSX, "mac.x86-sse2.exe", 612, ""},
	{"bee97c0ef8ff9d698e3d0034f8743bdd364bf8dabb5e3ec69ac70d71b43569f242640553e8623080263b7920e5eb649d", CPU_TYPE_X64_NVX, "mac.x64-sse2.exe", 612, ""},
	{"efb658f4064357afd9d72eea97d0660c4ec5303e845cef54eaa95134dd7f40524d330550689786d47e99ad63d464854e", CPU_TYPE_X64_AVX, "mac.x64-avx.exe",  612, ""},
	{"8e133974fef3c14e25ca661abb8cf1ee5a66d77738cab2b84b807efc4afa885f0586deb97a50e2a5be9cd85e70296e73", CPU_TYPE_X86_GEN, "mediainfo.i686.exe", 20090, ""},
	{"147f5a75caf915558cbb4a3f17d670a35406d8dbedba2709e6c13453595b0b567e9de37198b4add9525f01e4bf817ddd", CPU_TYPE_X86_SSX, "mediainfo.sse2.exe", 20090, ""},
	{"8037a0f8430e405dcf1d4d9dcf1a8d26d5d0fcc33aafff15100fa45ee4f1834f47e6f7a863bcee926f2fe384947bdab0", CPU_TYPE_X64_ALL, "mediainfo.x64.exe",  20090, ""},
	{"7e6346a057634ff07b2e1f427035324f7f02100cc996425990f87f71d767fce4c7b101588c7d944ba49cb2d7e51c9bdb", CPU_TYPE_ALL_ALL, "mpcdec.exe", 475, ""},
	{"c11107e563cf8131da2574cfb93c2661e343d615c17e3cdc95db130052e44fb2e188e1712f74c2eaac7ca42276398aa4", CPU_TYPE_X86_GEN, "mpg123.x86-i686.exe", 12604, ""},
	{"5847e83260552544c70e693e394dcf1928863350cf4bc6b3752728cda70ebec6f13db6bc8a3914a780bc283207fe4ba7", CPU_TYPE_X86_SSX, "mpg123.x86-sse2.exe", 12604, ""},
	{"5462b0cbca43cbc35c6ff31abf806397376ba35b2dfd12f3cbad4d7cbf481c9043cf2a0ac01f012d52e6014522e0a18d", CPU_TYPE_X64_NVX, "mpg123.x64-sse2.exe", 12604, ""},
	{"5572a331a8e0ee173369971c4e89720f631ce8b9beaa8bbb1758f258929074bb021b9ca5a4faa34667b8bb1bad7d07be", CPU_TYPE_X64_AVX, "mpg123.x64-avx.exe",  12604, ""},
	{"8e9b65f4bee39deceb303e5fdfe6875872fe450e29151ef3546128e7133a8eb3d14daae446e0c17c56ad3b969294367c", CPU_TYPE_ALL_GEN, "oggdec.i686.exe", 1101, ""},
	{"5fb1d7781de9449eed958a175305a9827b158f69cd83131da6b92cd78ee7ea3dcc47ed6b3ee58e2892fe6cf98a994730", CPU_TYPE_ALL_SSE, "oggdec.sse2.exe", 1101, ""},
	{"1f1f52553703afc11a428a4d5c9211f91cf9d39019ade3e049478e5a853b9a8c0a0d11304a04feb3a37cac6904244bab", CPU_TYPE_ALL_AVX, "oggdec.avx.exe",  1101, ""},
	{"5acb313abc310299987734edc9aa2290c24692284a79cdcac3ec85900284c80297c2a0b95c1f8ac01790dd181fb221db", CPU_TYPE_X86_GEN, "oggenc2.i686.exe", 288137603, "2018"},
	{"e0e91f07a427632a6fe2eed697a50311bf658cb61cb0f26eacf4b64487dac54022ab429587221a0375b6e3486f0e0bd1", CPU_TYPE_X86_SSX, "oggenc2.sse2.exe", 288137603, "2018"},
	{"3630078654f0ed43579d0eb11dba7752892afc361864d1620bcc49f233384c221827bbe88644f4a5b5aceeac4da7fe6c", CPU_TYPE_X64_ALL, "oggenc2.x64.exe",  288137603, "2018"},
	{"29494a82b09248e7f3d988c659fc16f0559c35f0ec4e55fd54dda87ffa172da0993cc58a7d112dc91e7feea360693cda", CPU_TYPE_ALL_GEN, "opusdec.i686.exe", 20190421, "v1.3.1"},
	{"aa5b532b58a0d84221627985edb06ad2dd12926e5514c27ee8bb6f508b5578ad63b08bee546b67199df0581f64f25e48", CPU_TYPE_ALL_SSE, "opusdec.sse2.exe", 20190421, "v1.3.1"},
	{"a32d191ac43dc0f3e527daf34201bd93a38fa34727d5aae12716ea248fa55a8b6932b33d12b03a68aaa7972950511406", CPU_TYPE_ALL_AVX, "opusdec.avx.exe",  20190421, "v1.3.1"},
	{"71a913bb3e2a0259f2badcac74447d5839d405ff85e31d0369e7a19119b64743b87c90851fb69d4c740bef478b04ece9", CPU_TYPE_ALL_GEN, "opusenc.i686.exe", 20190421, "v1.3.1"},
	{"90f9bb8ebbf4a26fb2a9af7640b596253e16cb20013472886347c0ceeb3b7c03ee60781287eb0d496ccad34db77e7d06", CPU_TYPE_ALL_SSE, "opusenc.sse2.exe", 20190421, "v1.3.1"},
	{"1e7a920786b02431924d6d0e1ae8ee748595a43c83947926d58ed026cc58157a093a6be56932b61c2249796953587995", CPU_TYPE_ALL_AVX, "opusenc.avx.exe",  20190421, "v1.3.1"},
	{"155b123845f797226d03c44d649de810b13001b70e55e3775559ee78b75c38c9284475d217a86752820cfe1c22f37f69", CPU_TYPE_X86_GEN, "refalac.i686.exe", 164, ""},
	{"2bdb8a6e4ad05669623173101ccc9287b9d8b6bb5b2062abaadda3e3cceda7040478ef2b404348b52e2232307f8c8f7c", CPU_TYPE_X86_SSX, "refalac.sse2.exe", 164, ""},
	{"3abae08b7be8d7e054bf48b7a7cbb874955a774dc2db69503490d59daf74db8adfa702401eeace6536810b0ac15dea84", CPU_TYPE_X64_ALL, "refalac.x64.exe",  164, ""},
	{"d041b60de6c5c6e77cbad84440db57bbeb021af59dd0f7bebd3ede047d9e2ddc2a0c14179472687ba91063743d23e337", CPU_TYPE_ALL_ALL, "shorten.exe", 361, ""},
	{"3206ebd1b1c6e5db422d7a84117a7ba8256208fc7104a6668d8856c1b6407882f25f1f39e37c8e33affb343300937d2e", CPU_TYPE_ALL_GEN, "sox.i686.exe", 1442, ""},
	{"16a71940aa5e9b393e83cdfb2a4dda4291ea01858f8ba338510013f25f4d7c53b9b5cffb86404ea49ef28e6795182fd5", CPU_TYPE_ALL_SSE, "sox.sse2.exe", 1442, "" },
	{"25585ca9e2e025d82d93341a9db8527eb0b4ce451dade607f9784a79ed30e050ced0824835d5467aa0bf0c6b8fe08612", CPU_TYPE_ALL_AVX, "sox.avx.exe",  1442, "" },
	{"5a4261e1b41a59d1a5bc92e1d2766422a67454d77e06ea29af392811b7b4704e0f3e494ab9cb6375ce9e39257867c5ed", CPU_TYPE_ALL_ALL, "speexdec.exe", 12, ""},
	{"a83628880da0b7519ec368a74a92da5a5099d8d46aa0583131f92d7321f47c9e16a1841b2a3fb8ffcca7205ef4b1bb0a", CPU_TYPE_ALL_ALL, "tta.exe", 21, ""},
	{"9e1ade2137ea5cee0ad4657971c314a372df3068594fbe4f77d45b9eb65fa7c69e55027b0df81b6fe072a220e9a8ba8a", CPU_TYPE_ALL_ALL, "valdec.exe", 100, "a"},
	{"572b9448bf4a338ecb9727951fdfcc5a219cc69896695cc96b9f6b083690e339910e41558968264a38992e45f2be152c", CPU_TYPE_ALL_ALL, "wma2wav.exe", 20111001, ""},
	{"5ef85aa6c6521161e19fc9eadd30bac82c3d0eee2374fd6ac543022181f7846ec2198ebe8bc84667e9b92a4a85d07fbb", CPU_TYPE_ALL_ALL, "wupdate.exe", 20171002, ""},
	{"6021b938769b09d05617c7e91e0cb6cc5f9e40c50cb470455afc21d466b37183b8675822b8797cbf98950a5262ec07a6", CPU_TYPE_X86_GEN, "wvunpack.x86-i686.exe", 5010, ""},
	{"fa322e127679ac6b5e833e120b590480b3ffc7ffa875705c04ed698f155dc23b40c8922d2f5f78ad1cc9342306289141", CPU_TYPE_X86_SSX, "wvunpack.x86-sse2.exe", 5010, ""},
	{"d4ca3085aae70160beab778a46a27643f1415bd803ddfecb2791fe964e4bff49ac5a891ef1852f26e867c42ddd6f8806", CPU_TYPE_X64_NVX, "wvunpack.x64-sse2.exe", 5010, ""},
	{"e2c32a765cbec41cc6e662447a1d70d8261753f2c93aca666bafddcc420c4eaee550d3c730123a84565cf43fffa26c35", CPU_TYPE_X64_AVX, "wvunpack.x64-avx.exe",  5010, ""},
	{NULL, NULL, NULL, NULL, NULL}
};

////////////////////////////////////////////////////////////
// AAC ENCODERS
////////////////////////////////////////////////////////////

typedef struct
{
	const char *const toolName;
	const char *const fileNames[5];
	const char *const checkArgs;
	const quint32     toolMinVersion;
	const quint32     verDigits;
	const quint32     verShift;
	const char *const verStr;
	const char *const regExpVer;
	const char *const regExpSig;
	const char *const regExpLib[3];
}
aac_encoder_t;

static const aac_encoder_t g_lamexp_aacenc[] =
{
	{ 
		"NeroAAC",
		{ "neroAacEnc.exe", "neroAacDec.exe", "neroAacTag.exe", NULL },
		"-help",
		lamexp_toolver_neroaac(), 4, 10, "v?.?.?.?",
		"Package\\s+version:\\s+(\\d)\\.(\\d)\\.(\\d)\\.(\\d)",
		"Nero\\s+AAC\\s+Encoder",
		{ NULL }
	},
	{
		"FhgAacEnc",
		{ "fhgaacenc.exe",  "enc_fhgaac.dll", "nsutil.dll", "libmp4v2.dll", NULL },
		NULL,
		lamexp_toolver_fhgaacenc(), 2, 0, "????-??-??",
		"fhgaacenc version (\\d+) by tmkk. Modified by Case (\\d+).",
		NULL,
		{ NULL }
	},
	{ 
		"FdkAacEnc",
		{ "fdkaac.exe", NULL },
		"--help",
		lamexp_toolver_fdkaacenc(), 3, 10, "v?.?.?",
		"fdkaac\\s+(\\d)\\.(\\d)\\.(\\d)",
		NULL,
		{ NULL }
	},
	{
		"QAAC",
		{ "qaac.exe", "libsoxr.dll", "libsoxconvolver.dll", NULL },
		"--check",
		lamexp_toolver_qaacenc(), 2, 100, "v?.??",
		"qaac (\\d)\\.(\\d+)",
		NULL,
		{ "libsoxr-\\d+\\.\\d+\\.\\d+", "libsoxconvolver\\s+\\d+\\.\\d+\\.\\d+" }
	},
	{
		"QAACx64",
		{ "qaac64.exe", "libsoxr64.dll", "libsoxconvolver64.dll", NULL },
		"--check",
		lamexp_toolver_qaacenc(), 2, 100, "v?.??",
		"qaac (\\d)\\.(\\d+)",
		NULL,
		{ "libsoxr-\\d+\\.\\d+\\.\\d+", "libsoxconvolver\\s+\\d+\\.\\d+\\.\\d+" }
	},
	{ NULL, { NULL }, NULL, 0, 0, 0, NULL, NULL, NULL, { NULL } }
};
