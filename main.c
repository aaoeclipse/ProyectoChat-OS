#include <stdio.h>
#include "cJSON.c"

//create a monitor with a list of supported resolutions
char* create_monitor(void);

/* return 1 if the monitor supports full hd, 0 otherwise */
int supports_full_hd(const char * const monitor);

int main() {
    char *a = create_monitor();
    printf("%s",a);
 //   cJSON *json = cJSON_Parse(string);
    int in;
    in = supports_full_hd(a);
    printf ("%d\n",in );

    printf("Prueba\n");

    char * buffer = 0;
    long length;
    FILE * f = fopen("/home/jazmitia/CLionProjects/untitled/prueba","rb");
    if (f){
        printf("entre al if\n");
        fseek(f,0,SEEK_END);
        length=ftell(f);
        fseek(f,0,SEEK_SET);
        buffer = malloc(length);
        if(buffer){
            fread(buffer,1,length,f);
            printf ("leido\n");
        }
        fclose(f);

    }
    if (buffer){
        printf("%s", buffer);
    }
    printf("fin prueba\n\n");
    cJSON *json = cJSON_Parse(buffer);
    int tamao = cJSON_GetArraySize(json);
    printf("tamao es %d\n\n",tamao);
    cJSON *aa = cJSON_GetArrayItem(json, 1);
    char *outt = cJSON_Print(aa);

    printf("%s\n", outt);
    printf("este es el primer o\n\n");



    char *out = cJSON_Print(json);
    cJSON_Delete(json);

    printf("%s\n", out);

    free(out);

    printf("otra prueba\n\n");
    printf("Mas pruebas\n");
    cJSON *img = NULL;
    cJSON *thm = NULL;
    int ids[4] = { 116, 943, 234, 38793 };
    cJSON *root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "Image", img = cJSON_CreateObject());
    cJSON_AddNumberToObject(img, "Width", 800);
    cJSON_AddNumberToObject(img, "Height", 600);
    cJSON_AddStringToObject(img, "Title", "View from 15th Floor");
    cJSON_AddItemToObject(img, "Thumbnail", thm = cJSON_CreateObject());
    cJSON_AddStringToObject(thm, "Url", "http:/*www.example.com/image/481989943");
    cJSON_AddNumberToObject(thm, "Height", 125);
    cJSON_AddStringToObject(thm, "Width", "100");
    cJSON_AddItemToObject(img, "IDs", cJSON_CreateIntArray(ids, 4));
    out = cJSON_Print(root);
    printf("%s\n\n", out);
    cJSON_Delete(root);
    free(out);
//-------------------------------------------------DEL PROYECTO---------------------------------------------------------

    //Estructura del json para el request del cliente
    printf("Estructura del json para el request del cliente\n");
    cJSON *Request_del_cliente = cJSON_CreateObject();
    cJSON_AddStringToObject(Request_del_cliente,"host","192.168.1.2");
    cJSON_AddStringToObject(Request_del_cliente,"origin","192.168.1.3");
    cJSON_AddStringToObject(Request_del_cliente,"user","<nombre>");
    out = cJSON_Print(Request_del_cliente);
    printf("%s\n\n", out);
    cJSON_Delete(Request_del_cliente);
    free(out);

    //Response del server del cliente
    cJSON *usr = NULL;
    printf("Estructura del json para el response del server del cliente");
    cJSON *Response_del_server_del_cliente = cJSON_CreateObject();
    cJSON_AddStringToObject(Response_del_server_del_cliente,"status","ok");
    cJSON_AddItemToObject(Response_del_server_del_cliente, "user", usr = cJSON_CreateObject());
    cJSON_AddStringToObject(usr,"id","ASdbjkxclz+asd?");
    cJSON_AddStringToObject(usr,"name","<nombre>");
    cJSON_AddStringToObject(usr,"status","<status>");
    out = cJSON_Print(Response_del_server_del_cliente);
    printf("%s\n\n", out);
    cJSON_Delete(Response_del_server_del_cliente);
    free(out);

    //En caso de un error
    printf("Estructura del json para ERROR\n");
    cJSON *Error= cJSON_CreateObject();
    cJSON_AddStringToObject(Error,"status","ERROR");
    cJSON_AddStringToObject(Error,"message","<mensaje de error>");
    out = cJSON_Print(Error);
    printf("%s\n\n", out);
    cJSON_Delete(Error);
    free(out);

    //Estructura de JSON de objeto de usuario
    printf("Estructura del json para objeto de usuario\n");
    cJSON *Objeto_Usuario= cJSON_CreateObject();
    cJSON_AddStringToObject(Objeto_Usuario,"id","ASdbjkxclz+asd?");
    cJSON_AddStringToObject(Objeto_Usuario,"name","<nombre>");
    cJSON_AddStringToObject(Objeto_Usuario,"status","<status>");
    out = cJSON_Print(Objeto_Usuario);
    printf("%s\n\n", out);
    cJSON_Delete(Objeto_Usuario);
    free(out);

    //Estructura de JSON de mensaje
    printf("Estructura del json de mensaje\n");
    cJSON *Mensaje= cJSON_CreateObject();
    cJSON_AddStringToObject(Mensaje,"action","SEND_MESSAGE");
    cJSON_AddStringToObject(Mensaje,"from","<idusuario>");
    cJSON_AddStringToObject(Mensaje,"to","<idusuario>");
    cJSON_AddStringToObject(Mensaje,"message","Hola");

    //cJSON *pruebas = cJSON_GetObjectItem(Mensaje, "action");
    //outt = cJSON_Print(pruebas);
    //printf("%s\n\n", outt);
    //cJSON_Delete(pruebas);
    //free(outt);
    //cJSON_DeleteItemFromObject(Mensaje,"action");

    out = cJSON_Print(Mensaje);
    printf("%s\n\n", out);
    cJSON_Delete(Mensaje);
    free(out);


    //Estructura de JSON de mensaje BROADCAST
    printf("Estructura del json de mensaje broadcast\n");
    cJSON *MensajeBroadcast= cJSON_CreateObject();
    cJSON_AddStringToObject(MensajeBroadcast,"action","BROADCAST_MESSAGE");
    cJSON_AddStringToObject(MensajeBroadcast,"from","<idusuario>");
    cJSON_AddStringToObject(MensajeBroadcast,"message","Hola");
    out = cJSON_Print(MensajeBroadcast);
    printf("%s\n\n", out);
    cJSON_Delete(MensajeBroadcast);
    free(out);

    //Estructura del JSON de recibir mensaje
    printf("Estructura del json de recibir mensaje\n");
    cJSON *Recieve_Message= cJSON_CreateObject();
    cJSON_AddStringToObject(Recieve_Message,"action","RECEIVE_MESSAGE");
    cJSON_AddStringToObject(Recieve_Message,"from","<id>");
    cJSON_AddStringToObject(Recieve_Message,"to","<id>");
    cJSON_AddStringToObject(Recieve_Message,"message","Hola");
    out = cJSON_Print(Recieve_Message);
    printf("%s\n\n", out);
    cJSON_Delete(Recieve_Message);
    free(out);


    //Estructura del Json para listar usuarios
    printf("Estructura del json de listar usuarios\n");
    cJSON *ListUsers= cJSON_CreateObject();
    cJSON_AddStringToObject(ListUsers,"action","LIST_USER");
    out = cJSON_Print(ListUsers);
    printf("%s\n\n", out);
    cJSON_Delete(ListUsers);
    free(out);

    //Estructura del Json para listar usuarios (server response)
    cJSON *usrs = NULL;
    printf("Estructura del json de listar usuarios (server response)\n");
    cJSON *ListUsersResponse= cJSON_CreateObject();
    cJSON_AddStringToObject(ListUsersResponse,"action","LIST_USER");
    cJSON_AddItemToObject(Response_del_server_del_cliente, "users", usrs = cJSON_CreateArray());
    cJSON *usuarios;
    //For cantidad de usuarios
    cJSON_AddItemToArray(usrs, usuarios = cJSON_CreateObject());
    cJSON *usuarios1;
    cJSON_AddItemToArray(usrs, usuarios1 = cJSON_CreateObject());
    cJSON_AddStringToObject(usuarios,"id","ASdbjkxclz+asd?");
    cJSON_AddStringToObject(usuarios,"name","<nombre>");
    cJSON_AddStringToObject(usuarios,"status","<status>");

    out = cJSON_Print(ListUsersResponse);
    printf("%s\n\n", out);
    cJSON_Delete(ListUsersResponse);
    free(out);


    //Estructura del Json para listar usuarios (server response) un usuario
    printf("Estructura del json de listar usuarios (Request) un usuario\n");
    cJSON *ListUsersRequestUnico= cJSON_CreateObject();
    cJSON_AddStringToObject(ListUsersRequestUnico,"action","LIST_USER");
    cJSON_AddStringToObject(ListUsersRequestUnico,"user","<id_del_usuario>");
    out = cJSON_Print(ListUsersRequestUnico);
    printf("%s\n\n", out);
    cJSON_Delete(ListUsersRequestUnico);
    free(out);


    //Estructura del Json para listar usuarios (server response) unico
    cJSON *usrsu = NULL;
    printf("Estructura del json de listar usuarios (server response)\n");
    cJSON *ListUsersResponseUnico= cJSON_CreateObject();
    cJSON_AddStringToObject(ListUsersResponse,"action","LIST_USER");
    cJSON_AddItemToObject(ListUsersResponseUnico, "users", usrsu = cJSON_CreateArray());
    cJSON *usuariosu;
    //For cantidad de usuarios
    cJSON_AddItemToArray(usrsu, usuariosu = cJSON_CreateObject());
    cJSON_AddStringToObject(usuariosu,"id","ASdbjkxclz+asd?");
    cJSON_AddStringToObject(usuariosu,"name","<nombre>");
    cJSON_AddStringToObject(usuariosu,"status","<status>");

    out = cJSON_Print(ListUsersResponseUnico);
    printf("%s\n\n", out);
    cJSON_Delete(ListUsersResponseUnico);
    free(out);

    //Estructura de JSON de cambio de estado
    printf("Estructura del json para cambio de estado\n");
    cJSON *RequestCambioEstado= cJSON_CreateObject();
    cJSON_AddStringToObject(RequestCambioEstado,"action","CHANGE_STATUS");
    cJSON_AddStringToObject(RequestCambioEstado,"user","<id_del_usuario>");
    cJSON_AddStringToObject(RequestCambioEstado,"status","<status>");
    out = cJSON_Print(RequestCambioEstado);
    printf("%s\n\n", out);
    cJSON_Delete(RequestCambioEstado);
    free(out);

    //RESPUESTAS DEL SERVIDOR
    //Estructura de JSON de respuesta sin errores
    printf("Estructura del json de respuesta sin errores\n");
    cJSON *RespuestaSinErrores= cJSON_CreateObject();
    cJSON_AddStringToObject(RespuestaSinErrores,"action","OK");
    out = cJSON_Print(RespuestaSinErrores);
    printf("%s\n\n", out);
    cJSON_Delete(RespuestaSinErrores);
    free(out);

    //Estructura de JSON de respuesta con errores
    char* mensaje = "mensaje de error";
    printf("Estructura del json de respuesta con errores\n");
    cJSON *RespuestaConErrores= cJSON_CreateObject();
    cJSON_AddStringToObject(RespuestaConErrores,"status","ERROR");
    cJSON_AddStringToObject(RespuestaConErrores,"message",mensaje);
    out = cJSON_Print(RespuestaConErrores);
    printf("%s\n\n", out);
    cJSON_Delete(RespuestaConErrores);
    free(out);



    return 0;



}





/* return 1 if the monitor supports full hd, 0 otherwise */
int supports_full_hd(const char * const monitor)
{
    const cJSON *resolution = NULL;
    const cJSON *resolutions = NULL;
    const cJSON *name = NULL;
    int status = 0;
    cJSON *monitor_json = cJSON_Parse(monitor);
    if (monitor_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        printf("Checking monitor \"%s\"\n", name->valuestring);
    }

    resolutions = cJSON_GetObjectItemCaseSensitive(monitor_json, "resolutions");
    cJSON_ArrayForEach(resolution, resolutions)
    {
        cJSON *width = cJSON_GetObjectItemCaseSensitive(resolution, "width");
        cJSON *height = cJSON_GetObjectItemCaseSensitive(resolution, "height");

        if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height))
        {
            status = 0;
            goto end;
        }

        if ((width->valuedouble == 1920) && (height->valuedouble == 1080))
        {
            status = 1;
            goto end;
        }
    }

    end:
    cJSON_Delete(monitor_json);
    return status;
}
//create a monitor with a list of supported resolutions
char* create_monitor(void)
{
    const unsigned int resolution_numbers[3][2] = {
            {1280, 720},
            {1920, 1080},
            {3840, 2160}
    };
    char *string = NULL;
    cJSON *name = NULL;
    cJSON *resolutions = NULL;
    cJSON *resolution = NULL;
    cJSON *width = NULL;
    cJSON *height = NULL;
    size_t index = 0;

    cJSON *monitor = cJSON_CreateObject();
    if (monitor == NULL)
    {
        goto end;
    }

    name = cJSON_CreateString("Awesome 4K");
    if (name == NULL)
    {
        goto end;
    }
    /* after creation was successful, immediately add it to the monitor,
     * thereby transfering ownership of the pointer to it */
    cJSON_AddItemToObject(monitor, "name", name);

    resolutions = cJSON_CreateArray();
    if (resolutions == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "resolutions", resolutions);

    for (index = 0; index < (sizeof(resolution_numbers) / (2 * sizeof(int))); ++index)
    {
        resolution = cJSON_CreateObject();
        if (resolution == NULL)
        {
            goto end;
        }
        cJSON_AddItemToArray(resolutions, resolution);

        width = cJSON_CreateNumber(resolution_numbers[index][0]);
        if (width == NULL)
        {
            goto end;
        }
        cJSON_AddItemToObject(resolution, "width", width);

        height = cJSON_CreateNumber(resolution_numbers[index][1]);
        if (height == NULL)
        {
            goto end;
        }
        cJSON_AddItemToObject(resolution, "height", height);
    }

    string = cJSON_Print(monitor);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }

    end:
    cJSON_Delete(monitor);
    return string;
}