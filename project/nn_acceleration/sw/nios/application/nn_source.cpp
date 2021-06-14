#include "nn_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
//#include "jsmn.cpp"
//#include "network.pb-c.h"

#ifndef _jsmn_
#define _jsmn_

#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

    // Header files

    /**
     * JSON type identifier. Basic types are:
     * 	o Object
     * 	o Array
     * 	o String
     * 	o Other primitive: number, boolean (true/false) or null
     */
    typedef enum {
        JSMN_UNDEFINED = 0,
        JSMN_OBJECT = 1,
        JSMN_ARRAY = 2,
        JSMN_STRING = 3,
        JSMN_PRIMITIVE = 4
    } jsmntype_t;

    enum jsmnerr {
        /* Not enough tokens were provided */
        JSMN_ERROR_NOMEM = -1,
        /* Invalid character inside JSON string */
        JSMN_ERROR_INVAL = -2,
        /* The string is not a full JSON packet, more bytes expected */
        JSMN_ERROR_PART = -3
    };

    /**
     * JSON token description.
     * type		type (object, array, string etc.)
     * start	start position in JSON data string
     * end		end position in JSON data string
     */
    typedef struct {
        jsmntype_t type;
        int start;
        int end;
        int size;
#ifdef JSMN_PARENT_LINKS
        int parent;
#endif
    } jsmntok_t;

    /**
     * JSON parser. Contains an array of token blocks available. Also stores
     * the string being parsed now and current position in that string
     */
    typedef struct {
        unsigned int pos; /* offset in the JSON string */
        unsigned int toknext; /* next token to allocate */
        int toksuper; /* superior token node, e.g parent object or array */
    } jsmn_parser;


    /**
     * Allocates a fresh unused token from the token pull.
     */
    static jsmntok_t* jsmn_alloc_token(jsmn_parser* parser,
        jsmntok_t* tokens, size_t num_tokens) {
        jsmntok_t* tok;
        if (parser->toknext >= num_tokens) {
            return NULL;
        }
        tok = &tokens[parser->toknext++];
        tok->start = tok->end = -1;
        tok->size = 0;
#ifdef JSMN_PARENT_LINKS
        tok->parent = -1;
#endif
        return tok;
    }

    /**
     * Fills token type and boundaries.
     */
    static void jsmn_fill_token(jsmntok_t* token, jsmntype_t type,
        int start, int end) {
        token->type = type;
        token->start = start;
        token->end = end;
        token->size = 0;
    }

    /**
     * Fills next available token with JSON primitive.
     */
    static int jsmn_parse_primitive(jsmn_parser* parser, const char* js,
        size_t len, jsmntok_t* tokens, size_t num_tokens) {
        jsmntok_t* token;
        int start;

        start = parser->pos;

        for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
            switch (js[parser->pos]) {
#ifndef JSMN_STRICT
                /* In strict mode primitive must be followed by "," or "}" or "]" */
            case ':':
#endif
            case '\t': case '\r': case '\n': case ' ':
            case ',': case ']': case '}':
                goto found;
            }
            if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
                parser->pos = start;
                return JSMN_ERROR_INVAL;
            }
        }
#ifdef JSMN_STRICT
        /* In strict mode primitive must be followed by a comma/object/array */
        parser->pos = start;
        return JSMN_ERROR_PART;
#endif

    found:
        if (tokens == NULL) {
            parser->pos--;
            return 0;
        }
        token = jsmn_alloc_token(parser, tokens, num_tokens);
        if (token == NULL) {
            parser->pos = start;
            return JSMN_ERROR_NOMEM;
        }
        jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
#ifdef JSMN_PARENT_LINKS
        token->parent = parser->toksuper;
#endif
        parser->pos--;
        return 0;
    }

    /**
     * Fills next token with JSON string.
     */
    static int jsmn_parse_string(jsmn_parser* parser, const char* js,
        size_t len, jsmntok_t* tokens, size_t num_tokens) {
        jsmntok_t* token;

        int start = parser->pos;

        parser->pos++;

        /* Skip starting quote */
        for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
            char c = js[parser->pos];

            /* Quote: end of string */
            if (c == '\"') {
                if (tokens == NULL) {
                    return 0;
                }
                token = jsmn_alloc_token(parser, tokens, num_tokens);
                if (token == NULL) {
                    parser->pos = start;
                    return JSMN_ERROR_NOMEM;
                }
                jsmn_fill_token(token, JSMN_STRING, start + 1, parser->pos);
#ifdef JSMN_PARENT_LINKS
                token->parent = parser->toksuper;
#endif
                return 0;
            }

            /* Backslash: Quoted symbol expected */
            if (c == '\\' && parser->pos + 1 < len) {
                int i;
                parser->pos++;
                switch (js[parser->pos]) {
                    /* Allowed escaped symbols */
                case '\"': case '/': case '\\': case 'b':
                case 'f': case 'r': case 'n': case 't':
                    break;
                    /* Allows escaped symbol \uXXXX */
                case 'u':
                    parser->pos++;
                    for (i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0'; i++) {
                        /* If it isn't a hex character we have an error */
                        if (!((js[parser->pos] >= 48 && js[parser->pos] <= 57) || /* 0-9 */
                            (js[parser->pos] >= 65 && js[parser->pos] <= 70) || /* A-F */
                            (js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */
                            parser->pos = start;
                            return JSMN_ERROR_INVAL;
                        }
                        parser->pos++;
                    }
                    parser->pos--;
                    break;
                    /* Unexpected symbol */
                default:
                    parser->pos = start;
                    return JSMN_ERROR_INVAL;
                }
            }
        }
        parser->pos = start;
        return JSMN_ERROR_PART;
    }

    /**
     * Parse JSON string and fill tokens.
     */
    int jsmn_parse(jsmn_parser* parser, const char* js, size_t len,
        jsmntok_t* tokens, unsigned int num_tokens) {
        int r;
        int i;
        jsmntok_t* token;
        int count = parser->toknext;

        for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
            char c;
            jsmntype_t type;

            c = js[parser->pos];
            switch (c) {
            case '{': case '[':
                count++;
                if (tokens == NULL) {
                    break;
                }
                token = jsmn_alloc_token(parser, tokens, num_tokens);
                if (token == NULL)
                    return JSMN_ERROR_NOMEM;
                if (parser->toksuper != -1) {
                    tokens[parser->toksuper].size++;
#ifdef JSMN_PARENT_LINKS
                    token->parent = parser->toksuper;
#endif
                }
                token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
                token->start = parser->pos;
                parser->toksuper = parser->toknext - 1;
                break;
            case '}': case ']':
                if (tokens == NULL)
                    break;
                type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
                if (parser->toknext < 1) {
                    return JSMN_ERROR_INVAL;
                }
                token = &tokens[parser->toknext - 1];
                for (;;) {
                    if (token->start != -1 && token->end == -1) {
                        if (token->type != type) {
                            return JSMN_ERROR_INVAL;
                        }
                        token->end = parser->pos + 1;
                        parser->toksuper = token->parent;
                        break;
                    }
                    if (token->parent == -1) {
                        if (token->type != type || parser->toksuper == -1) {
                            return JSMN_ERROR_INVAL;
                        }
                        break;
                    }
                    token = &tokens[token->parent];
                }
#else
                for (i = parser->toknext - 1; i >= 0; i--) {
                    token = &tokens[i];
                    if (token->start != -1 && token->end == -1) {
                        if (token->type != type) {
                            return JSMN_ERROR_INVAL;
                        }
                        parser->toksuper = -1;
                        token->end = parser->pos + 1;
                        break;
                    }
                }
                /* Error if unmatched closing bracket */
                if (i == -1) return JSMN_ERROR_INVAL;
                for (; i >= 0; i--) {
                    token = &tokens[i];
                    if (token->start != -1 && token->end == -1) {
                        parser->toksuper = i;
                        break;
                    }
                }
#endif
                break;
            case '\"':
                r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
                if (r < 0) return r;
                count++;
                if (parser->toksuper != -1 && tokens != NULL)
                    tokens[parser->toksuper].size++;
                break;
            case '\t': case '\r': case '\n': case ' ':
                break;
            case ':':
                parser->toksuper = parser->toknext - 1;
                break;
            case ',':
                if (tokens != NULL && parser->toksuper != -1 &&
                    tokens[parser->toksuper].type != JSMN_ARRAY &&
                    tokens[parser->toksuper].type != JSMN_OBJECT) {
#ifdef JSMN_PARENT_LINKS
                    parser->toksuper = tokens[parser->toksuper].parent;
#else
                    for (i = parser->toknext - 1; i >= 0; i--) {
                        if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT) {
                            if (tokens[i].start != -1 && tokens[i].end == -1) {
                                parser->toksuper = i;
                                break;
                            }
                        }
                    }
#endif
                }
                break;
#ifdef JSMN_STRICT
                /* In strict mode primitives are: numbers and booleans */
            case '-': case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case 't': case 'f': case 'n':
                /* And they must not be keys of the object */
                if (tokens != NULL && parser->toksuper != -1) {
                    jsmntok_t* t = &tokens[parser->toksuper];
                    if (t->type == JSMN_OBJECT ||
                        (t->type == JSMN_STRING && t->size != 0)) {
                        return JSMN_ERROR_INVAL;
                    }
                }
#else
                /* In non-strict mode every unquoted value is a primitive */
            default:
#endif
                r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
                if (r < 0) return r;
                count++;
                if (parser->toksuper != -1 && tokens != NULL)
                    tokens[parser->toksuper].size++;
                break;

#ifdef JSMN_STRICT
                /* Unexpected char in strict mode */
            default:
                return JSMN_ERROR_INVAL;
#endif
            }
        }

        if (tokens != NULL) {
            for (i = parser->toknext - 1; i >= 0; i--) {
                /* Unmatched opened object or array */
                if (tokens[i].start != -1 && tokens[i].end == -1) {
                    return JSMN_ERROR_PART;
                }
            }
        }

        return count;
    }

    /**
     * Creates a new parser based over a given  buffer with an array of tokens
     * available.
     */
    void jsmn_init(jsmn_parser* parser) {
        parser->pos = 0;
        parser->toknext = 0;
        parser->toksuper = -1;
    }

    static int json_key_check(const char* json, jsmntok_t* tok, const char* s) {
        if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
            return 0;
        }
        return 1;
    }

#ifdef __cplusplus
}
#endif

#endif /*_jsmn_*/

#define LOOKUP_SIZE 4096
#define SIGMOID_CUTOFF 45.0

// const double SIGMOID_DOM_MIN = -20.0;
// const double SIGMOID_DOM_MAX = 20.0;

// double sigmoid_lookup[LOOKUP_SIZE];

// Activation functions
extern inline double ffnn_activation_linear(double x) { return x; }

extern inline double ffnn_activation_threshold(double x) { return x > 0; }

extern inline double ffnn_activation_relu(double x) {
    if (x > 0) return x;
    return 0;
}

extern inline double ffnn_activation_sigmoid(double x) {
    if (x < -SIGMOID_CUTOFF) return 0.0;
    if (x > SIGMOID_CUTOFF) return 1.0;
    return 1.0 / (1.0 + exp(-x));
}

/*extern inline double ffnn_activation_softmax(double x) {
    if (x > 12) return 162754.791419;
    return exp(x);
}
*/
char* dupstring(const char* s, size_t n)
{
    size_t len = strnlen(s, n);
    char* ne = (char*)malloc(len + 1);
    if (ne == NULL)
        return NULL;
    ne[len] = '\0';
    return (char*)memcpy(ne, s, len);
}
NetworkLayer* create_layer(int number_of_nodes, int input_length, double* weights, double* biases, const char* activation) {
    // Layer activation function defaults to sigmoid
    NetworkLayer* network_layer = (NetworkLayer*)malloc(sizeof(NetworkLayer));
    network_layer->number_of_nodes = number_of_nodes;
    network_layer->input_length = input_length;

    // Initialize activation function
    if (strcmp(activation, "linear") == 0) {
        network_layer->activation_type = ACTIVATION_TYPE_LINEAR;
        network_layer->activation_func = ffnn_activation_linear;
    }
    else if (strcmp(activation, "relu") == 0) {
        network_layer->activation_type = ACTIVATION_TYPE_RELU;
        network_layer->activation_func = ffnn_activation_relu;
    }
    else if (strcmp(activation, "threshold") == 0) {
        network_layer->activation_type = ACTIVATION_TYPE_THRESHOLD;
        network_layer->activation_func = ffnn_activation_threshold;
    }
    /*else if (strcmp(activation, "softmax") == 0) {
        network_layer->activation_type = ACTIVATION_TYPE_SOFTMAX;
        network_layer->activation_func = ffnn_activation_softmax;
    }
    */
    else {
        network_layer->activation_type = ACTIVATION_TYPE_SIGMOID;
        printf("NetworkLayer:create_layer:loading a sigmoid function as default\n");
        network_layer->activation_func = ffnn_activation_sigmoid;
    }
    // Initialize weights, biases and output
    network_layer->weights = weights; //(double*) realloc(weights, number_of_nodes * input_length * sizeof(double));
    network_layer->biases = biases; //(double*) realloc(biases, number_of_nodes * sizeof(double));
    network_layer->output = (double*)malloc(number_of_nodes * sizeof(double));
    return network_layer;
}

void free_layer(NetworkLayer* network_layer) {
    if (network_layer) {
        free(network_layer->weights);
        free(network_layer->biases);
        free(network_layer->output);
    }
    free(network_layer);
}

double* run_layer(NetworkLayer* network_layer, double* input) {
    double res;
    for (int node = 0; node < network_layer->number_of_nodes; node++) {
        res = network_layer->biases[node];
        for (int i = 0; i < network_layer->input_length; i++) {
            res += network_layer->weights[node * network_layer->input_length + i] * input[i];
        }
        network_layer->output[node] = network_layer->activation_func(res);
    }
   /* if (network_layer->activation_type == ACTIVATION_TYPE_SOFTMAX) {
        double soft_sum = 0;
        for (int node = 0; node < network_layer->number_of_nodes; node++)
            soft_sum += network_layer->output[node];
        for (int node = 0; node < network_layer->number_of_nodes; node++)
            network_layer->output[node] /= soft_sum;
    }
    */
    return network_layer->output;
}

/*
void create_ffnn_sigmoid_lookup() {
    const double f = (SIGMOID_DOM_MAX - SIGMOID_DOM_MIN) / LOOKUP_SIZE;
    double interval = LOOKUP_SIZE / (SIGMOID_DOM_MAX - SIGMOID_DOM_MIN);
    for (int i = 0; i < LOOKUP_SIZE; ++i) {
        sigmoid_lookup[i] = ffnn_activation_sigmoid(SIGMOID_DOM_MIN + f * i);
    }
}
*/



Network* create_network_from_json(const char* json_network) {
    //printf("Network:create_network_from_json:\n %s \n\n", json_network);

    jsmn_parser p;
    jsmntok_t tokens[MAXIMUM_JSON_TOKEN_SIZE];

    jsmn_init(&p);
    int element_count = jsmn_parse(&p, json_network, strlen(json_network), tokens, MAXIMUM_JSON_TOKEN_SIZE);//sizeof(tokens)/sizeof(tokens[0]));
    if (element_count < 0) {
        printf("Network:create_network_from_json:Failed to parse JSON: %d\n", element_count);
        return NULL;
    }

    /* Assume the top-level element is an object */
    if (element_count < 1 || tokens[0].type != JSMN_OBJECT) {
        printf("Network:create_network_from_json:JSON object expected\n");
        return NULL;
    }

    Network* network = (Network*)calloc(1, sizeof(Network));
    // network -> number_of_layers = 0;

    //printf("Network:create_network_from_json:parameters------------:\n");
    // Declare temporary parameters for constructing network layers
    char* activation_universal = NULL;
    char** activations = NULL;// Use alloca or malloc+free
    int number_of_layers = 0;
    int activation_size = 0;
    double** layer_biases = NULL;// Use alloca or malloc+free
    int* layer_biases_vector_sizes = NULL;// Use alloca or malloc+free
    int layer_biases_size = 0;
    double** layer_weights = NULL;// Use alloca or malloc+free
    int* layer_weights_cols = NULL;// Use alloca or malloc+free
    int* layer_weights_rows = NULL;// Use alloca or malloc+free
    int* layer_weights_grid_sizes = NULL;// Use alloca or malloc+free
    int layer_weights_size = 0;

    int token_index = 1;
    /* Loop over all keys of the root object */
    while (token_index < element_count) {
        if (json_key_check(json_network, &tokens[token_index], "activations") == 0) {
            jsmntok_t* activation_values = &tokens[++token_index];
            if (activation_values->type != JSMN_ARRAY) {
                printf("ERROR:Network:create_network_from_json:Invalid activation format:activations is not an array!");
                free(network);
                return NULL;
            }
            //printf("-- activations:\n");
            ++token_index; // Unwrap array.
            activation_size = activation_values->size;
            activations = (char**)alloca(activation_size * sizeof(char*));
            for (int i = 0; i < activation_values->size; i++) {
                jsmntok_t* value_token = &tokens[token_index + i];
                activations[i] = dupstring(json_network + value_token->start, value_token->end - value_token->start);
                //printf("---- %s\n", activations[i]);
            }
            token_index += activation_values->size;
        }
        else if (json_key_check(json_network, &tokens[token_index], "activation") == 0) {
            /// We may use dupstring() to fetch string value
            activation_universal = dupstring(json_network + tokens[token_index + 1].start, tokens[token_index + 1].end - tokens[token_index + 1].start);
            //printf("-- universal activation: %s\n", activation_universal);
            token_index += 2;
        }
        else if (json_key_check(json_network, &tokens[token_index], "layerSizes") == 0) {
            jsmntok_t* json_layer_sizes = &tokens[++token_index];
            if (json_layer_sizes->type != JSMN_ARRAY) {
                printf("ERROR:Network:create_network_from_json:Invalid network format:layerSizes is not an array!");
                free(network);
                return NULL;
            }
            network->layer_sizes = (int*)alloca(json_layer_sizes->size * sizeof(int));
            number_of_layers = json_layer_sizes->size - 1;
            //printf("-- numberOfLayers:%i (Not including input layer)\n", number_of_layers);
            ++token_index; // Unwrap array.
            //printf("-- layerSizes:\n");
            for (int i = 0; i < json_layer_sizes->size; i++) {
                jsmntok_t* value_token = &tokens[token_index + i];

                char* layer_size_str = dupstring(json_network + value_token->start, value_token->end - value_token->start);
                //printf("---- %s\n", layer_size_str);
                network->layer_sizes[i] = atoi(layer_size_str);
                free(layer_size_str);

                if (network->layer_sizes[i] == 0) {
                    printf("ERROR:Network:create_network_from_json:Invalid node size in layerSizes is not an integer: %s!", layer_size_str);
                    free(network);
                    return NULL;
                }
            }
            token_index += json_layer_sizes->size;
        }
        else if (json_key_check(json_network, &tokens[token_index], "biases") == 0) {
            jsmntok_t* bias_objects = &tokens[++token_index];
            if (bias_objects->type != JSMN_ARRAY) {
                printf("ERROR:Network:create_network_from_json:Invalid network format:biases is not an array!");
                free(network);
                return NULL;
            }
            ++token_index; // Unwrap array.
            //printf("-- biases:\n");
            layer_biases_size = bias_objects->size;
            layer_biases = (double**)alloca(bias_objects->size * sizeof(double*));
            layer_biases_vector_sizes = (int*)alloca(bias_objects->size * sizeof(int));

            for (int bias_ind = 0; bias_ind < bias_objects->size; bias_ind++) {
                jsmntok_t* bias_object = &tokens[token_index];
                ++token_index; // Unwrap biasObject

                for (int bias_object_token = 0; bias_object_token < bias_object->size; bias_object_token++) {
                    if (json_key_check(json_network, &tokens[token_index + 1], "vector")) {
                        ++token_index; // access vector value
                        jsmntok_t* bias_object_vector = &tokens[token_index];
                        layer_biases[bias_ind] = (double*)alloca(bias_object_vector->size * sizeof(double));
                        layer_biases_vector_sizes[bias_ind] = bias_object_vector->size;
                        //printf("---- vector:\n");
                        for (int i = 0; i < bias_object_vector->size; i++) {
                            jsmntok_t* value = &tokens[token_index + i + 1];
                            char* bias_value_str = dupstring(json_network + value->start, value->end - value->start);
                            layer_biases[bias_ind][i] = atof(bias_value_str);
                            free(bias_value_str);
                            //printf("------ %lf\n", layer_biases[bias_ind][i]);
                        }
                        token_index += bias_object_vector->size;
                    }
                    else {
                        printf("ERROR:Network:create_network_from_json:Unexpected key in bias object: %.*s\n", tokens[token_index].end - tokens[token_index].start, json_network + tokens[token_index].start);
                        free(network);
                        return NULL;
                    }
                }
                ++token_index;// Go to next object
            }
        }
        else if (json_key_check(json_network, &tokens[token_index], "weights") == 0) {
            jsmntok_t* weight_objects = &tokens[++token_index];
            if (weight_objects->type != JSMN_ARRAY) {
                printf("ERROR:Network:create_network_from_json:Invalid network format:weights is not an array!");
                free(network);
                return NULL;
            }
            ++token_index; // Unwrap array.
            //printf("-- weights:\n");
            layer_weights_size = weight_objects->size;
            layer_weights = (double**)alloca(weight_objects->size * sizeof(double*));
            layer_weights_cols = (int*)alloca(weight_objects->size * sizeof(int));
            layer_weights_rows = (int*)alloca(weight_objects->size * sizeof(int));
            layer_weights_grid_sizes = (int*)alloca(weight_objects->size * sizeof(int));

            for (int weight_ind = 0; weight_ind < weight_objects->size; weight_ind++) {
                jsmntok_t* weight_object = &tokens[token_index];
                ++token_index; // Unwrap weightObject

                for (int weight_object_token_ind = 0; weight_object_token_ind < weight_object->size; weight_object_token_ind++) {
                    if (json_key_check(json_network, &tokens[token_index], "col") == 0) {
                        char* col_str = dupstring(json_network + tokens[token_index + 1].start, tokens[token_index + 1].end - tokens[token_index + 1].start);
                        layer_weights_cols[weight_ind] = atoi(col_str);
                        free(col_str);
                        //printf("---- col: %i\n", layer_weights_cols[weight_ind]);
                        token_index += 2;
                    }
                    else if (json_key_check(json_network, &tokens[token_index], "row") == 0) {
                        char* row_str = dupstring(json_network + tokens[token_index + 1].start, tokens[token_index + 1].end - tokens[token_index + 1].start);
                        layer_weights_rows[weight_ind] = atoi(row_str);
                        free(row_str);
                        //printf("---- row: %i\n", layer_weights_rows[weight_ind]);
                        token_index += 2;
                    }
                    else if (json_key_check(json_network, &tokens[token_index], "grid") == 0) {
                        ++token_index; // access vector value
                        jsmntok_t* weight_object_grid = &tokens[token_index];
                        layer_weights[weight_ind] = (double*)alloca(weight_object_grid->size * sizeof(double));
                        //printf("---- grid:\n");
                        layer_weights_grid_sizes[weight_ind] = weight_object_grid->size;
                        for (int i = 0; i < weight_object_grid->size; i++) {
                            jsmntok_t* value = &tokens[token_index + i + 1];
                            char* weight_value_str = dupstring(json_network + value->start, value->end - value->start);
                            layer_weights[weight_ind][i] = atof(weight_value_str);
                            free(weight_value_str);
                            //printf("------ %lf\n", layer_weights[weight_ind][i]);
                        }
                        token_index += weight_object_grid->size;
                    }
                    else {
                        printf("ERROR:Network:create_network_from_json:Unexpected key in weight object: %.*s\n", tokens[token_index].end - tokens[token_index].start, json_network + tokens[token_index].start);
                        free(network);
                        return NULL;
                    }
                }
                ++token_index;// Go to next object
            }
        }
        else {
            //printf("Unexpected key: %.*s\n", tokens[token_index].end-tokens[token_index].start, json_network + tokens[token_index].start);
            //++ token_index;
            printf("ERROR:Network:create_network_from_json:Unexpected key JSON object: %.*s\n", tokens[token_index].end - tokens[token_index].start, json_network + tokens[token_index].start);
            free(network);
            return NULL;
        }
    }
    // printf("DEBUG:Memory activation: %s\n", activations[0]);
    // printf("DEBUG:Memory bias: %lf\n", layer_biases[0][1]);
    // printf("DEBUG:Memory weights: %lf\n", layer_weights[0][1]);

    // Validate network local variables
    if (number_of_layers > 0 && number_of_layers == layer_weights_size && number_of_layers == layer_biases_size) {
        if (activation_size == number_of_layers || activation_universal != NULL) {
            network->layers = (NetworkLayer**)calloc(number_of_layers, sizeof(NetworkLayer*));
            network->number_of_layers = number_of_layers;
            int success = 0; char* layer_activation = NULL;
            for (int i = 0; i < number_of_layers; i++) {
                if (layer_biases_vector_sizes[i] != network->layer_sizes[i + 1]) {
                    printf("ERROR:Network:create_network_from_json:Invalid bias vector size:\n");
                    success = 1;
                    break;
                }
                if (layer_weights_cols[i] != network->layer_sizes[i]) {
                    printf("ERROR:Network:create_network_from_json:Invalid weight col size:\n");
                    success = 1;
                    break;
                }
                if (layer_weights_rows[i] != network->layer_sizes[i + 1]) {
                    printf("ERROR:Network:create_network_from_json:Invalid weight col size:\n");
                    success = 1;
                    break;
                }
                if (layer_weights_grid_sizes[i] != network->layer_sizes[i] * network->layer_sizes[i + 1]) {
                    printf("ERROR:Network:create_network_from_json:Invalid weight grid size:\n");
                    success = 1;
                    break;
                }
                // Construct neural network layers
                if (activation_size == number_of_layers) layer_activation = activations[i];
                else layer_activation = activation_universal;
                double* weights = (double*)malloc(layer_weights_grid_sizes[i] * sizeof(double));
                double* biases = (double*)malloc(layer_biases_vector_sizes[i] * sizeof(double));

                /*
                for(int j = 0; j < layer_weights_grid_sizes[i]; j ++) {
                    printf("--- DEBUG: weight %lf\n", layer_weights[i][j]);
                    weights[j] = layer_weights[i][j];
                    printf("--- DEBUG: copied weight %lf\n", weights[j]);
                }

                printf("DEBUG: weights %lf, %lf, %lf, %lf\n",layer_weights[i][0],layer_weights[i][1],layer_weights[i][2],layer_weights[i][3]);
                */
                for (int k = 0; k < layer_biases_vector_sizes[i]; k++) biases[k] = layer_biases[i][k];
                memcpy(weights, layer_weights[i], layer_weights_grid_sizes[i] * sizeof(double));
                memcpy(biases, layer_biases[i], layer_biases_vector_sizes[i] * sizeof(double));
                NetworkLayer* layer = create_layer(network->layer_sizes[i + 1], network->layer_sizes[i], weights, biases, layer_activation);
                // printf("DEBUG: copied weights %lf, %lf, %lf, %lf\n",weights[0], weights[1],weights[2],weights[3]);
                network->layers[i] = layer;
            }
            if (activation_size > 0) {
                for (int i = 0; i < activation_size; i++) free(activations[i]);
            }
            if (success == 0) {
                network->input_length = network->layer_sizes[0];
                network->output_length = network->layer_sizes[number_of_layers];
                network->output = network->layers[number_of_layers - 1]->output;
                return network;
            }
        }
        else {
            printf("ERROR:Network:create_network_from_json:Invalid activation:\n");
        }
    }
    else {
        printf("ERROR:Network:create_network_from_json:Number of layers does not match layer biases and layer weights:\n");
    }

    if (activation_size > 0) {
        for (int i = 0; i < activation_size; i++) free(activations[i]);
    }
    // Failed to create a network, free memory and return
    free_network(network);
    return NULL;

    // Free up memory
    //free(activations);
    //free(layer_biases);
    //free(layer_weights);
    //free(layer_weight_cols);
    //free(layer_weight_rows);
}

void free_network(Network* network) {
    if (network != NULL && network->number_of_layers > 0) {
        for (int i = 0; i < network->number_of_layers; i++) free_layer(network->layers[i]);
    }
    free(network);
}

double* run_network(Network* network, double* input) {
    // Activate first layer with input
    run_layer(network->layers[0], input);
    for (int i = 1; i < network->number_of_layers; i++) {
        run_layer(network->layers[i], network->layers[i - 1]->output);
    }
    return network->output;
}

