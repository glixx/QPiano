SET( TRANSLATIONS_FILES
     qpiano_ca.ts
     qpiano_cs.ts
     qpiano_da.ts
     qpiano_de.ts
     qpiano_es.ts
     qpiano_fr.ts
     qpiano_gl.ts
     qpiano_is.ts
     qpiano_it.ts
     qpiano_ja.ts
     qpiano_nb.ts
     qpiano_nl.ts
     qpiano_pl.ts
     qpiano_ru.ts
     qpiano_sk.ts
     qpiano_sv.ts
     qpiano_uk.ts
     qpiano_zh.ts
     music_ru.ts
     music_uk.ts
)

if(INSTALL_ALL_LANGS)
file (GLOB TRANSLATIONS_FILES "*.ts")   
endif()

get_filename_component(full_path_blank_ts ${CMAKE_CURRENT_SOURCE_DIR}/qpiano_blank.ts ABSOLUTE)
list(REMOVE_ITEM TRANSLATIONS_FILES ${full_path_blank_ts})

get_filename_component(full_path_blank_ts ${CMAKE_CURRENT_SOURCE_DIR}/music_blank.ts ABSOLUTE)
list(REMOVE_ITEM TRANSLATIONS_FILES ${full_path_blank_ts})

FIND_PACKAGE(Qt5 REQUIRED COMPONENTS LinguistTools)

QT5_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
ADD_CUSTOM_TARGET(translations ALL DEPENDS ${QM_FILES})

IF (UNIX AND NOT APPLE)
    INSTALL( FILES ${QM_FILES} DESTINATION ${DATA_DIR}/translations )
    INSTALL( FILES langs.json DESTINATION ${DATA_DIR}/translations )
ENDIF (UNIX AND NOT APPLE)

IF (WIN32)
    INSTALL( FILES ${QM_FILES} DESTINATION . )
    INSTALL( FILES langs.json DESTINATION . )
ENDIF (WIN32)
