/* derived from symlink.c */
/* prints names of attrs for an entity defined in an EXPRESS file */
/* symlink.c author: Don Libes, NIST, 20-Mar-1993 */

#include <stdlib.h>
#include <sys/param.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "express/express.h"
#include <express/scope.h>
#include <express/variable.h>

char * entityName, _buf[512] = { 0 };

void my_usage() {
    EXPRESSusage( 0 );
    printf( "\t----\n\t-a <entity>: print attrs for <entity>\n" );
    exit( 2 );
}

void print_attrs( Express model ) {
    DictionaryEntry de;
    Schema s;
    Entity e;
    Linked_List attrs;

    if( !entityName ) {
        my_usage();
    }

    printf( "File: %s\n  ", model->u.express->filename );

    DICTdo_init( model->symbol_table, &de );
    while( 0 != ( s = DICTdo( &de ) ) ) {
        printf( "Schema %s\n", s->symbol.name );
        e = DICTlookup( s->symbol_table, entityName );
        if( e ) {
            printf( "    Entity %s\n", e->symbol.name );
            attrs = ENTITYget_all_attributes( e );
            LISTdo( attrs, attr, Variable ) {
                printf( "     %s%s, from %s\n", ( attr->initializer ? "*" : " " ), attr->name->symbol.name, attr->type->superscope->symbol.name );
            } LISTod
        } else {
            printf( "\tnot found.\n" );
        }
        LISTfree( attrs );
    }
    printf( "\n" );
    exit( 0 );
}

int attr_arg( int i, char * arg ) {
    const char * src = arg;
    int count = 0;
    if( ( char )i == 'a' ) {
        entityName = _buf;
        while( *src ) {
            _buf[count] = tolower( *src );
            src++;
            count++;
            if( count == 511 ) {
                break;
            }
        }
        if( count == 0 ) {
            entityName = 0;
        }
    } else if( !entityName ) {
        //if libexpress comes across an unrecognized arg that isn't '-a' and if the entityName isn't set
        return 1; // print usage and exit
    }
    return 0;
}

void EXPRESSinit_init() {
    entityName = 0;
    EXPRESSbackend = print_attrs;
    ERRORusage_function = my_usage;
    strcat( EXPRESSgetopt_options, "a:" );
    EXPRESSgetopt = attr_arg;
}
