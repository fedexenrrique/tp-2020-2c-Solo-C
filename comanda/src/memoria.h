/*
 * memoria.h
 *
 *  Created on: 13 sep. 2020
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "serializar.h"

#define SIZE_PAGINA 32

typedef struct {
	char * nombre_restaurante;
	t_list * tabla_pedidos;
}t_restaurante;

typedef struct{
	int    nro_frame      ;
	void * direccion_frame;
}t_frame;


typedef struct {
	bool     esta_en_memoria_principal  ;
	t_frame* frame                      ;
	t_frame* frame_swap    		        ;
	void *   contenido                  ;
	bool     modificado                 ;
	uint64_t last_used                  ;

}t_pagina_comida;



//1 t_tabla_segmentos X restaurante  (contiene los t_segmneto)
//1 t_segmento  X cada pedido   (contiene los t_pagina)
//1 t_pagina contiene un t_comida

void   * p_inicio_memoria_principal 		   ;
void   * p_fin_memoria_principal   			   ;
void   * p_inicio_memoria_swap      		   ;
void   * p_fin_memoria_swap     			   ;
int      size_memoria_principal    			   ;
int      size_memoria_swap      			   ;
char   * algoritmo_remplazo                    ;
t_list * list_pointer_memory_principal		   ;
t_list * list_pointer_memory_swap   		   ;
t_list * lista_restarurantes				   ;
t_list * tabla_frames_libres				   ;
t_list * tabla_frames_libres_swap 			   ;


void             iniciar_memoria                  (                        );
void             crear_paginas_memoria            (t_list *,int ,t_list*   );
void *           reservar_memoria_inicial         (int                     );
t_restaurante *  crear_tabla_segmentos_restaurante(char *                  );
bool             agregar_pedido_a_tabla_segmentos (t_restaurante *,uint32_t);
void             copiar_pagina_en_memoria         (void *   , t_comida  *  );
void             leer_pagina_en_memoria           (void *    , t_comida *  );
t_pagina_comida* cargar_pagina_a_memoria_principal(t_pagina_comida *       );

t_frame *  seleccionar_victima_en_memoria_principal     ();
t_frame *  utilizar_algoritmo_remplazo_lru              ();
t_frame *  utilizar_algoritmo_remplazo_clock_mejorado   ();

t_frame * buscar_frame_libre (t_list*);
void      iniciar_dump_cache(int     );
void      dampear_memoria   (        );
uint64_t  timestamp         (void    );


#endif /* MEMORIA_H_ */
