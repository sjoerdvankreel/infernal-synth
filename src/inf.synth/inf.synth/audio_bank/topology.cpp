#include <inf.synth/audio_bank/topology.hpp>
#include <inf.base/shared/support.hpp>
#include <inf.base/topology/part_descriptor.hpp>

#include <ranges>

using namespace inf::base;

namespace inf::synth {

static list_item const vaudio_route_inputs[vaudio_route_input::count] = {
  { "{49276AD4-960F-49D4-9B74-A8E4D5A86B6B}", "Off" },
  { "{BA7D11D6-27D3-460C-849E-4FFA756290DF}", "Osc" }, // all
  { "{A3D43958-5219-4D44-A51A-54853EF6F457}", "Osc" }, // any 
  { "{1E01C31B-3160-49B6-A88F-7A532AE5B407}", "FX" } };
static list_item const vaudio_route_outputs[vaudio_route_output::count] = {
  { "{F91EF1BE-F439-44AA-9B6C-EAB5A186ECCB}", "Off" },
  { "{514ED3EB-9195-412D-8E50-9EB89D80CF1D}", "FX" },
  { "{E05A9233-EF1F-4267-85DF-04F33DFC611B}", "Voice" } };
static list_item const gaudio_route_inputs[gaudio_route_input::count] = {
  { "{18620BA6-61E3-4AFE-88D0-37DBE443B67D}", "Off" }, 
  { "{973FADF8-945E-4196-9E9A-3B422B09ED21}", "Voice" },
  { "{C4FFD4E3-9BBC-4D9F-8083-FAD0268BD4C3}", "Ext" },
  { "{213616DC-90FD-4128-A517-E63C97457D15}", "FX" } };
static list_item const gaudio_route_outputs[gaudio_route_output::count] = {
  { "{AE02B0B8-10DD-44F9-912C-C94E46D7615D}", "Off" },
  { "{5F5E1D56-311E-4558-9C90-B368E8BAF237}", "FX" },
  { "{A781CA9A-B8AA-448A-924B-D16C31108CB5}", "Master" } };
 
static std::vector<list_item> const vaudio_route_input_items =  
multi_list_items(vaudio_route_inputs, nullptr, vaudio_route_input_counts, vaudio_route_input::count, false);
static std::vector<list_item> const gaudio_route_input_items =
multi_list_items(gaudio_route_inputs, nullptr, gaudio_route_input_counts, gaudio_route_input::count, false);
static std::vector<list_item> const vaudio_route_output_items =
multi_list_items(vaudio_route_outputs, nullptr, vaudio_route_output_counts, vaudio_route_output::count, false);
static std::vector<list_item> const gaudio_route_output_items = 
multi_list_items(gaudio_route_outputs, nullptr, gaudio_route_output_counts, gaudio_route_output::count, false);

param_descriptor const 
vaudio_bank_params[vaudio_bank_param::count] = 
{  
  { "{2E9F0478-B911-43DF-BB51-0C5836E4853F}", { { "In", "Input 1" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{A3A59082-CF73-4C28-A3FC-037729C9CB42}", { { "In", "Input 2" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{A8E6882A-8945-4F59-92B9-78004EAF5818}", { { "In", "Input 3" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{7DFD9AF0-7419-4B06-B875-F18D9C344D42}", { { "In", "Input 4" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{2A252D70-9750-4385-8405-AE1EAF5E8018}", { { "In", "Input 5" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{34E1B446-EF1B-4715-9993-64A177769033}", { { "In", "Input 6" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{05F53161-12D1-41E5-8A29-3EC34D4CD93E}", { { "In", "Input 7" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{3B3104C4-D8B1-45D5-B821-3E85253332E7}", { { "In", "Input 8" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{71A9A8A5-7972-445B-9BBC-2594C89AF784}", { { "In", "Input 9" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{3DA3631F-F0F6-4803-8328-D8B45ABBF5E6}", { { "In", "Input 10" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{E9F5974A-BC98-44F6-84DD-745C5406784C}", { { "In", "Input 11" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{58B59E16-BF4A-4363-9889-C0E52B90A30E}", { { "In", "Input 12" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{34AFC85D-4DC6-4854-8488-78D731F3E436}", { { "In", "Input 13" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{DC4BA5B6-276F-48F9-97A2-5C41AD693863}", { { "In", "Input 14" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{6B06F326-AD6C-4621-9D13-961F10738CA5}", { { "In", "Input 15" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{00F7898D-0302-4158-9B8C-1E0ADFAC212C}", { { "In", "Input 16" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{49CB3F65-AF7D-4877-B644-A40B57A9A5D9}", { { "In", "Input 17" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{3B388F09-0187-4BBA-9216-025A152F78EB}", { { "In", "Input 18" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{60EB55F3-C0BE-415E-81E8-A8F9A0E2DB0A}", { { "In", "Input 19" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{046D3FD3-DC35-4751-88E1-8EB36381885F}", { { "In", "Input 20" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{A3CAE4EF-902F-43FA-B70E-001EADC0AE98}", { { "In", "Input 21" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{2742DCF8-B48D-4843-9165-3948E7694F7D}", { { "In", "Input 22" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{73088AAA-C421-4D7F-82BA-5D544AF8B21F}", { { "In", "Input 23" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{628F3388-E430-4CAC-96BF-013A4DC35584}", { { "In", "Input 24" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 } } },
  { "{295DC5F0-FB32-4D43-8799-D79F23FD3AA9}", { { "Out", "Output 1" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{843EB41C-199F-4FAC-ACC4-841B3663248D}", { { "Out", "Output 2" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{D2E34E63-DE9E-4620-924D-1897614BF983}", { { "Out", "Output 3" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{C2C5B9BC-81B6-4D3E-947E-E0B900447BDF}", { { "Out", "Output 4" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{80FF4399-33CE-4A65-9A2C-C48271EAACDD}", { { "Out", "Output 5" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{DAA1F891-5A2A-47A7-B923-61932694B951}", { { "Out", "Output 6" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{07210305-0D9C-42A0-8C56-77168093B6C9}", { { "Out", "Output 7" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{174F31F7-27DB-4016-A66E-A4E0676D1FB2}", { { "Out", "Output 8" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{ADA1A9E9-291E-473B-A752-8B65F452CC50}", { { "Out", "Output 9" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{69DA2D69-ACAD-4917-B975-FAAF3D5D4AE5}", { { "Out", "Output 10" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{F27F11FC-35B9-4273-951A-B29504A555C7}", { { "Out", "Output 11" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{C13E8FD1-AD65-417D-9D9F-5923D7E5CCA7}", { { "Out", "Output 12" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{33BC23D6-041D-4574-B2CD-0311489F69D9}", { { "Out", "Output 13" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{AE9058E4-708B-4475-B91F-4A69FBDB2AB8}", { { "Out", "Output 14" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{61C773F1-B760-4B20-8C53-AC893764021B}", { { "Out", "Output 15" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{22A13DFF-C04C-42D6-BD5F-B9DBBFA0836A}", { { "Out", "Output 16" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{D394B046-D9EA-4FDA-90F2-69D0DCAD36A5}", { { "Out", "Output 17" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{C381D265-0FE3-4899-82F1-F7D4FE982488}", { { "Out", "Output 18" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{AC5F4638-21CB-411B-8771-54A79A0C6051}", { { "Out", "Output 19" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{175E961C-9C27-40AB-B078-367024A1FFE0}", { { "Out", "Output 20" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{59C152B9-7795-44CB-8DE3-EE2BDE00EBF1}", { { "Out", "Output 21" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{D0B06AFA-21DE-4069-A581-BB6DC7FFBD35}", { { "Out", "Output 22" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{72571851-3363-4F05-AC28-B1B97F5D2E68}", { { "Out", "Output 23" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{D026288B-CDAC-4A78-A2BB-9678750C6102}", { { "Out", "Output 24" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 } } },
  { "{A3B15FE9-56DB-493B-A4E1-31A004F3C937}", { { "Amt", "Amount 1" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{810B55DF-6230-45C7-B478-7A3569DC9127}", { { "Amt", "Amount 2" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{6721B1EC-9688-48A3-B5B8-0ADD0A9CF16B}", { { "Amt", "Amount 3" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{51E69AB1-37F7-4361-84B9-2F1727F66C4A}", { { "Amt", "Amount 4" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{88EACE54-DAFA-4EEF-A7A0-65464C54A66E}", { { "Amt", "Amount 5" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{782750CE-319E-4ED2-906B-106C72A9A85C}", { { "Amt", "Amount 6" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{BE2208C7-C208-42A5-B1CC-3990F057CCD3}", { { "Amt", "Amount 7" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{840B1A85-0B41-42C2-BC0F-A349AB123EAC}", { { "Amt", "Amount 8" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{7BCE1683-E3D9-4824-9F8C-FDF21610DF27}", { { "Amt", "Amount 9" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{193FAE9B-48FE-4E40-A7F6-1035BA6A356C}", { { "Amt", "Amount 10" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{211D8D04-3A24-48A4-9E21-66E70880D085}", { { "Amt", "Amount 11" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{CAFF2917-4FCC-4F19-ADC6-A589B68D24DF}", { { "Amt", "Amount 12" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{2E25001F-95F4-4B41-B1C8-2224D07FBBF1}", { { "Amt", "Amount 13" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{F535F293-EDA5-4994-AFD7-EA1F0B7F6321}", { { "Amt", "Amount 14" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{8D5540FA-46ED-4B89-8098-A6A885877220}", { { "Amt", "Amount 15" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{604BE895-D3D8-4997-9C1C-4A97B99ABF80}", { { "Amt", "Amount 16" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{8AB5708A-DCDA-46DF-8E4A-8D9A62A34734}", { { "Amt", "Amount 17" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{38328C19-B61F-45B3-8174-DEDBDCD1032F}", { { "Amt", "Amount 18" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{8EC9D902-B57A-4B64-8E53-711C7DC16ABB}", { { "Amt", "Amount 19" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{F1A932EF-08C4-4D59-BADF-FCF3049A9F46}", { { "Amt", "Amount 20" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{2193B25A-6F04-4967-862B-AC4552AB6F89}", { { "Amt", "Amount 21" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{A52BAF05-3F9C-42E0-8408-ED9004454085}", { { "Amt", "Amount 22" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{03D23588-9D0E-46BB-87F1-E2C1E12B6300}", { { "Amt", "Amount 23" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{8317276D-DBD4-4F9D-A3DD-0F852A474195}", { { "Amt", "Amount 24" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{91996C1F-4510-4BC1-97BA-135C9881263F}", { { "Bal", "Balance 1" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{265269F4-F6DF-474E-A696-44EE01681C65}", { { "Bal", "Balance 2" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{64C90CE2-2AC0-4675-8F99-B88E807F712A}", { { "Bal", "Balance 3" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{01B6309E-B045-48A7-9BC1-8E828A528A3F}", { { "Bal", "Balance 4" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{77F5DF0C-B9E8-4059-AE8B-75B9D6E3E0CE}", { { "Bal", "Balance 5" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{EF2AD9AF-0A3C-4DD7-8650-5DD6974C4625}", { { "Bal", "Balance 6" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{70515B3A-ED00-472F-8AE1-09E8A510AF49}", { { "Bal", "Balance 7" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{1A7318C4-44D2-4A7A-9C3C-673C0C646EC5}", { { "Bal", "Balance 8" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{9A90952C-2BB2-4BB0-9A38-24F7761835AB}", { { "Bal", "Balance 9" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{86D91CD6-0121-4545-A041-7C4BF711F654}", { { "Bal", "Balance 10" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{5932BB77-554F-4B53-9266-72E48650F472}", { { "Bal", "Balance 11" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{C3ADE2F2-365A-45AF-A127-BA17A1578D3A}", { { "Bal", "Balance 12" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{08E30815-B17D-4A1F-B85B-F89A67F767EF}", { { "Bal", "Balance 13" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{D166F745-F0AE-40B6-9075-92852A29F804}", { { "Bal", "Balance 14" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{AE994FD7-762E-456B-A87C-56482A46BB87}", { { "Bal", "Balance 15" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{88B4E98C-E9FB-4818-8691-7C9D2E01467E}", { { "Bal", "Balance 16" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{DB4D3723-CE4F-4AAE-BDCC-4C39A9F5D693}", { { "Bal", "Balance 17" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{892138A5-9813-4843-9764-97B705EF5CBC}", { { "Bal", "Balance 18" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{3713A58E-0D12-4DF6-A9FA-BA10B2FAFAA8}", { { "Bal", "Balance 19" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{3011A343-092D-4291-9E19-263F87E1F4D7}", { { "Bal", "Balance 20" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{86AED61A-BA71-436D-A2FE-AA17D17D9F17}", { { "Bal", "Balance 21" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{C1DCDF81-8CE3-4EE3-BCF8-F25273B72C59}", { { "Bal", "Balance 22" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{1EF528E3-94F9-45F4-9727-B925FEB3EDBC}", { { "Bal", "Balance 23" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{0519F6BF-B79F-4716-9FC6-A0D0BE95E945}", { { "Bal", "Balance 24" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } }
};

param_descriptor const  
gaudio_bank_params[gaudio_bank_param::count] = 
{
  { "{CD5CD403-259F-4B25-9C33-E246931E973B}", { { "In", "Input 1" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{E83D6E12-47C4-4738-8CFA-A18CC8B86C67}", { { "In", "Input 2" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{EF89DAE6-59F6-4B5E-BA7F-F9F9F4FA64C0}", { { "In", "Input 3" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{3F1E4A05-2C73-418D-B490-841106011784}", { { "In", "Input 4" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{2D03FFCF-FD1D-42F7-B95B-BE3262A2900F}", { { "In", "Input 5" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{47517379-6D53-48C7-BFD0-A2D582C7971F}", { { "In", "Input 6" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{CD5CD403-259F-4B25-9C33-E246931E973B}", { { "In", "Input 7" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{E83D6E12-47C4-4738-8CFA-A18CC8B86C67}", { { "In", "Input 8" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{EF89DAE6-59F6-4B5E-BA7F-F9F9F4FA64C0}", { { "In", "Input 9" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{3F1E4A05-2C73-418D-B490-841106011784}", { { "In", "Input 10" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{2D03FFCF-FD1D-42F7-B95B-BE3262A2900F}", { { "In", "Input 11" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{47517379-6D53-48C7-BFD0-A2D582C7971F}", { { "In", "Input 12" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{CD5CD403-259F-4B25-9C33-E246931E973B}", { { "In", "Input 13" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{E83D6E12-47C4-4738-8CFA-A18CC8B86C67}", { { "In", "Input 14" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{EF89DAE6-59F6-4B5E-BA7F-F9F9F4FA64C0}", { { "In", "Input 15" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{3F1E4A05-2C73-418D-B490-841106011784}", { { "In", "Input 16" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{2D03FFCF-FD1D-42F7-B95B-BE3262A2900F}", { { "In", "Input 17" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{47517379-6D53-48C7-BFD0-A2D582C7971F}", { { "In", "Input 18" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 } } },
  { "{6AD76233-62A6-4F5A-ADCB-797786E00C54}", { { "Out", "Output 1" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{8BEAB138-D485-480C-B2BE-146354C5A2F9}", { { "Out", "Output 2" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{33E9FBE4-4654-4B92-AD03-5EF2EC3FEEF2}", { { "Out", "Output 3" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{9F28A209-5A25-4665-BB73-557BE1F3CC05}", { { "Out", "Output 4" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{5452CCEE-C962-4239-9F5C-015445598B72}", { { "Out", "Output 5" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{E6B0B447-2412-43D1-9BED-66BC67620272}", { { "Out", "Output 6" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{6AD76233-62A6-4F5A-ADCB-797786E00C54}", { { "Out", "Output 7" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{8BEAB138-D485-480C-B2BE-146354C5A2F9}", { { "Out", "Output 8" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{33E9FBE4-4654-4B92-AD03-5EF2EC3FEEF2}", { { "Out", "Output 9" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{9F28A209-5A25-4665-BB73-557BE1F3CC05}", { { "Out", "Output 10" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{5452CCEE-C962-4239-9F5C-015445598B72}", { { "Out", "Output 11" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{E6B0B447-2412-43D1-9BED-66BC67620272}", { { "Out", "Output 12" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{6AD76233-62A6-4F5A-ADCB-797786E00C54}", { { "Out", "Output 13" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{8BEAB138-D485-480C-B2BE-146354C5A2F9}", { { "Out", "Output 14" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{33E9FBE4-4654-4B92-AD03-5EF2EC3FEEF2}", { { "Out", "Output 15" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{9F28A209-5A25-4665-BB73-557BE1F3CC05}", { { "Out", "Output 16" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{5452CCEE-C962-4239-9F5C-015445598B72}", { { "Out", "Output 17" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{E6B0B447-2412-43D1-9BED-66BC67620272}", { { "Out", "Output 18" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 } } },
  { "{E444E539-65EF-449F-8407-EB128C6082B8}", { { "Amt", "Amount 1" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{FBCCD63D-1D9A-444C-A622-D9D3E8A771C7}", { { "Amt", "Amount 2" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{C147FB0B-25A9-44F9-88E2-77CA415F83BB}", { { "Amt", "Amount 3" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{E394ACC9-DA07-43DF-9BF7-02A57BF3F758}", { { "Amt", "Amount 4" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{8A1B591B-62AD-4E53-9256-7C4BFB15525F}", { { "Amt", "Amount 5" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{2D1B1704-9B20-44EE-AC48-E8B09FBA9488}", { { "Amt", "Amount 6" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{E444E539-65EF-449F-8407-EB128C6082B8}", { { "Amt", "Amount 7" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{FBCCD63D-1D9A-444C-A622-D9D3E8A771C7}", { { "Amt", "Amount 8" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{C147FB0B-25A9-44F9-88E2-77CA415F83BB}", { { "Amt", "Amount 9" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{E394ACC9-DA07-43DF-9BF7-02A57BF3F758}", { { "Amt", "Amount 10" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{8A1B591B-62AD-4E53-9256-7C4BFB15525F}", { { "Amt", "Amount 11" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{2D1B1704-9B20-44EE-AC48-E8B09FBA9488}", { { "Amt", "Amount 12" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{E444E539-65EF-449F-8407-EB128C6082B8}", { { "Amt", "Amount 13" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{FBCCD63D-1D9A-444C-A622-D9D3E8A771C7}", { { "Amt", "Amount 14" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{C147FB0B-25A9-44F9-88E2-77CA415F83BB}", { { "Amt", "Amount 15" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{E394ACC9-DA07-43DF-9BF7-02A57BF3F758}", { { "Amt", "Amount 16" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{8A1B591B-62AD-4E53-9256-7C4BFB15525F}", { { "Amt", "Amount 17" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{2D1B1704-9B20-44EE-AC48-E8B09FBA9488}", { { "Amt", "Amount 18" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{3E7BA2FC-2984-4B2F-9936-754BEE44CFCE}", { { "Bal", "Balance 1" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{D2672F61-8811-420C-A4CA-1ED78D49AC55}", { { "Bal", "Balance 2" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{10073537-70C6-40FB-8F53-0ABB2C594944}", { { "Bal", "Balance 3" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{EBBBAC9E-3D8F-482A-A41B-32CB47324647}", { { "Bal", "Balance 4" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{80D09377-31F5-4162-A785-D0B841FCDBA6}", { { "Bal", "Balance 5" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{7E72F00C-2C26-4BAD-98E5-ADC7168852F7}", { { "Bal", "Balance 6" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } }
  { "{3E7BA2FC-2984-4B2F-9936-754BEE44CFCE}", { { "Bal", "Balance 7" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{D2672F61-8811-420C-A4CA-1ED78D49AC55}", { { "Bal", "Balance 8" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{10073537-70C6-40FB-8F53-0ABB2C594944}", { { "Bal", "Balance 9" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{EBBBAC9E-3D8F-482A-A41B-32CB47324647}", { { "Bal", "Balance 10" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{80D09377-31F5-4162-A785-D0B841FCDBA6}", { { "Bal", "Balance 11" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{7E72F00C-2C26-4BAD-98E5-ADC7168852F7}", { { "Bal", "Balance 12" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } }
  { "{3E7BA2FC-2984-4B2F-9936-754BEE44CFCE}", { { "Bal", "Balance 13" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{D2672F61-8811-420C-A4CA-1ED78D49AC55}", { { "Bal", "Balance 14" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{10073537-70C6-40FB-8F53-0ABB2C594944}", { { "Bal", "Balance 15" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{EBBBAC9E-3D8F-482A-A41B-32CB47324647}", { { "Bal", "Balance 16" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{80D09377-31F5-4162-A785-D0B841FCDBA6}", { { "Bal", "Balance 17" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
  { "{7E72F00C-2C26-4BAD-98E5-ADC7168852F7}", { { "Bal", "Balance 18" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } }
};

} // namespace inf::synth