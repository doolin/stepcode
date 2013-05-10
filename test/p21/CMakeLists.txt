CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#test part 21 files

set( schema_name_tests_head "
ISO-10303-21;
HEADER;
FILE_DESCRIPTION(('SC test file'),'2;1');
FILE_NAME('test.stp','2011-11-29T',('mp'),(''),'0','1','2');
")
set( schema_name_tests_tail "
ENDSEC;
DATA;
#1=DRAUGHTING_PRE_DEFINED_COLOUR('green');
#2=COLOUR_RGB('',1.1E-2,1.2E-2,1.E0);
ENDSEC;
END-ISO-10303-21;
")

set( schema_name_good     "FILE_SCHEMA(('AUTOMOTIVE_DESIGN'));" )
set( schema_name_good_asn "FILE_SCHEMA(('AUTOMOTIVE_DESIGN { 1 0 10303 214 1 1 1 1 }'));" )
set( schema_name_mismatch "FILE_SCHEMA(('DOES_NOT_MATCH'));" )

set( p21read_ap214 "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p21read_sdai_AP214E3_2010" )

file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/exch_file_good.p21      "${schema_name_tests_head}${schema_name_good}${schema_name_tests_tail}" )
file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/exch_file_good_asn.p21  "${schema_name_tests_head}${schema_name_good_asn}${schema_name_tests_tail}" )
file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/exch_file_mismatch.p21  "${schema_name_tests_head}${schema_name_mismatch}${schema_name_tests_tail}" )

#these compare the schema name in a p21 file with than in the sdai lib
add_test( test_good_schema_name      ${p21read_ap214}    ${CMAKE_CURRENT_BINARY_DIR}/exch_file_good.p21 )
add_test( test_good_schema_name_asn  ${p21read_ap214}    ${CMAKE_CURRENT_BINARY_DIR}/exch_file_good_asn.p21 )
add_test( test_mismatch_schema_name  ${p21read_ap214}    ${CMAKE_CURRENT_BINARY_DIR}/exch_file_mismatch.p21 )
add_test( test_ignore_schema_name    ${p21read_ap214} -i ${CMAKE_CURRENT_BINARY_DIR}/exch_file_mismatch.p21 )

#these test for required attrs that are missing
add_test( test_missing_and_required        ${p21read_ap214}    ${CMAKE_CURRENT_SOURCE_DIR}/missing_and_required.p21 )
add_test( test_missing_and_required_strict ${p21read_ap214} -s ${CMAKE_CURRENT_SOURCE_DIR}/missing_and_required.p21 )

#test acceptance of comments within p21 entity, i.e. FILE_NAME( /* name */ 'ferrari sharknose', ...);
add_test( test_p21_entity_internal_comment ${p21read_ap214}    ${CMAKE_CURRENT_SOURCE_DIR}/comments.p21 )


#not sure how multiple schema names should be handled for testing, by p21read, or by the schema and stepeditor libs
#add_test( multi_schema )
#rotator_asm:
# FILE_SCHEMA(('CONFIG_CONTROL_DESIGN', 'GEOMETRIC_VALIDATION_PROPERTIES_MIM','SHAPE_APPEARANCE_LAYER_MIM'));

set_tests_properties( test_good_schema_name test_good_schema_name_asn test_mismatch_schema_name
                      test_ignore_schema_name test_missing_and_required test_missing_and_required_strict test_p21_entity_internal_comment
                      PROPERTIES DEPENDS build_cpp_sdai_AP214E3_2010 LABELS exchange_file )

set_tests_properties( test_mismatch_schema_name test_missing_and_required_strict PROPERTIES WILL_FAIL TRUE )

#check for a segmentation fault with selects. the files are a subset of the 210e3 schema and of SurfaceMountFlasher.stp, respectively
#p21read will report errors with the file, but they don't matter as long as there isn't a segfault.
BUILD_A_SCHEMA(${SC_SOURCE_DIR}/test/misc/select_segfault_210e3.exp)
add_test( test_select_segfault ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p21read_sdai_select_segfault_210e3
                               ${CMAKE_CURRENT_SOURCE_DIR}/select_segfault_210e3.stp )
set_tests_properties( test_select_segfault PROPERTIES DEPENDS build_cpp_sdai_select_segfault_210e3 LABELS exchange_file WILL_FAIL TRUE )

#multiple inheritance where two attrs with same name are inherited; one is derived
BUILD_A_SCHEMA(${SC_SOURCE_DIR}/test/unitary_schemas/multiple_inheritance_derived.exp)
set( MI_DERIVED_HEAD "
ISO-10303-21;
HEADER;
FILE_DESCRIPTION((''),'2;1');
FILE_NAME('', '', (''), (''), '', '', '');
FILE_SCHEMA(('multiple_inheritance_derived'));
ENDSEC;
DATA;
")
set( MI_DERIVED_TAIL "
ENDSEC;
END-ISO-10303-21;
" )
#nr is appended to the test name; instance is a p21 instance; fail is the value of the WILL_FAIL test property
FUNCTION( MI_DERIVED_TEST nr instance fail )
  set( test_file "${CMAKE_CURRENT_BINARY_DIR}/multiple_inheritance_derived_${nr}.stp" )
  file( WRITE ${test_file} "${MI_DERIVED_HEAD}${instance}${MI_DERIVED_TAIL}" )
  add_test( test_multiple_inheritance_derived_${nr} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p21read_sdai_multiple_inheritance_derived ${test_file} )
  set_tests_properties( test_multiple_inheritance_derived_${nr} PROPERTIES DEPENDS build_cpp_sdai_multiple_inheritance_derived LABELS exchange_file WILL_FAIL ${fail} )
ENDFUNCTION( MI_DERIVED_TEST nr instance fail )

MI_DERIVED_TEST( 1 "#1=AB(*);      /* should be invalid */" TRUE )
MI_DERIVED_TEST( 2 "#2=AB(.BEE.);  /* should be  valid  */" FALSE )
MI_DERIVED_TEST( 3 "#3=A(*);       /* should be invalid */" TRUE )
MI_DERIVED_TEST( 4 "#4=B(.BEE.);   /* should be  valid  */" FALSE )
MI_DERIVED_TEST( 5 "#5=A();        /* should be  valid  */" FALSE )
MI_DERIVED_TEST( 6 "#6=B();        /* should be invalid */" TRUE )
