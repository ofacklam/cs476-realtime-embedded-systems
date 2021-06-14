import torch
torch.set_grad_enabled(False)


def where_one_zero(value,pos,neg,ops):
    mask = torch.empty(value.shape)
    mask_neg=mask.new_full(value.shape, neg)
    mask_pos=mask.new_full(value.shape, pos)
    return mask_pos.where(ops,mask_neg)

class MiniNN(object):
    def __init__(self):
        self.sorted_layers =[]
        self.params = {}
# initialize NN module, ordered_layers is list of layer_stack params is cascaded tuple of percepton parameters and their gradients for each layer  
    def forward(self, *inputs):
        raise NotImplementedError
# forward function is defined in child class
    def backpass(self, loss):
# backpropagation of dE/dZ for each layer
        layers = []
        for name, v in self.__dict__.items():
              if isinstance(v, Layer_stack):
                    layer = v
                    layer.name = name
                    layers.append((layer.order, layer))
        self.sorted_layers = [l[1] for l in sorted(layers, key=lambda x: x[0])]

# back propagate by the following order
        last_layer = self.sorted_layers[-1]
        last_layer.data_layer["out"].set_error(loss)
        dz_back=loss
        for layer in self.sorted_layers[::-1]:
            layer.data_layer["out"].set_error(dz_back)
            if layer==last_layer:
                layer.data_layer["out"].set_error(loss)
            grads = layer.backward()
            dz_back=layer.data_layer['in']._error
            for k in layer.param_layer.keys():
#params has been initialized
                self.params[layer.name]["grads"][k][:] = grads[k]
#define the sequential block and add the layers and their parameters in sequential block to the main NN module
    def sequential(self, *layers):
      # sequential layers can be input in tuple or list
        assert isinstance(layers, (list, tuple))
        for i, l in enumerate(layers):
            self.__setattr__("layer_%i" % i, l)
        return Seq_Layers(layers)

    def __call__(self, *args):
        return self.forward(*args)
# initialize parameters of each layer
    def __setattr__(self, key, value):
        if isinstance(value, Layer_stack):
            layer = value
            self.params[key] = {
                "paras": layer.param_layer,
                "grads": {k: torch.empty(layer.param_layer[k].shape) for k in layer.param_layer.keys()}
            }
        object.__setattr__(self, key, value)

# store input and output of each layer and dE/dZ of each layer including delta i.e., differential of loss against output of last layer.
class General_unit:
    def __init__(self, v):
        self.data = v
        self._error = torch.empty(v.shape) # for backpropagation of the last layer
        self.info = {}
    def set_error(self, error):
        assert self._error.shape == error.shape
        self._error[:] = error

#sequential block
class Seq_Layers:
    def __init__(self, layers):
        assert isinstance(layers, (list, tuple))
        self.layers = layers
    def forward(self, x):
        for l in self.layers:
            x = l.forward(x)
        return x

    def __call__(self, x):
        return self.forward(x)
        
# perceptron layer followed by activation function or linear full conection layer
class Layer_stack:
    def __init__(self,
                 n_in,
                 n_out,
                 activation=None,
                 w_initializer=None,
                 b_initializer=None,
                 use_bias=True,
                 ):
      # shape of weight matrix
        self.w_shape=(n_in, n_out)
        self._n_in = n_in
        self._n_out = n_out
        self.order = None
        self.name = None
        self._x = None
        self._wx_b = None
        self._activated = None
      # store input and output of layer 
        self.data_layer = {}
      # store parameters of layer 
        self.param_layer = {}
        self.w = torch.empty(self.w_shape)
        self.param_layer["w"] = self.w
        if use_bias:
            shape = [1]*len(self.w_shape)
            shape[-1] = self.w_shape[-1]   
            self.b = torch.empty(shape)
            self.param_layer["b"] = self.b
        self.use_bias = use_bias
      # configurable activation function and initialization for parameters
        if activation is None:
            self._a = Linear()
        else:
            self._a = activation

        if w_initializer is None:
            Constant(0.001).initialize(self.w)
        else:
            w_initializer.initialize(self.w)

        if use_bias:
            if b_initializer is None:
                Constant(0.001).initialize(self.b)
            else:
                b_initializer.initialize(self.b)
    #obtain input and transform it into General_unit class             
    def transform_input(self, x):
        if not isinstance(x, General_unit):
            x = x.type(torch.float32)
            x = General_unit(x)
            x.info["input_order"] = 0

        self.data_layer["in"] = x
        # x is General_unit, extract _x value from x.data
        self.order = x.info["input_order"]
        #obtain order from input
        _x = x.data
        return _x
    #
    def attach_output(self, out):
        out = General_unit(out)
        out.info["input_order"] = self.order + 1
        self.data_layer["out"] = out  
        return out
    # forward and ouput of layer is input of next layer
    def forward(self, x):
        self._x = self.transform_input(x)
        self._wx_b = self._x.matmul(self.w)
        if self.use_bias:
            self._wx_b += self.b
        self._activated = self._a(self._wx_b)   # if act is None, act will be Linear
        layer_output = self.attach_output(self._activated)
        return layer_output
   # backward and ouput of layer is input of next layer
    def backward(self):
        # dw, db
        dz = self.data_layer["out"]._error
        dz *= self._a.derivative(self._wx_b)
        grads = {"w": self._x.transpose(0,1).matmul(dz)}
        if self.use_bias:
            grads["b"] = dz.sum(axis=0, keepdims=True)
        # pass error to the layer before
        self.data_layer["in"].set_error(dz.matmul(self.w.T))     
        return grads
    def __call__(self, x):
        return self.forward(x)

class RandomNormal:
    def __init__(self, mean=0., std=0.2):
        self._mean = mean
        self._std = std

    def initialize(self, x):
        x[:] = x.normal_()

class RandomUniform:
    def __init__(self,low=0.0,high=1.0):
        self._low = low
        self._high = high
    def initialize(self, x):
      x[:] = x.uniform_(self._low,self._high)
class Constant:
    def __init__(self, v):
        self._v = v

    def initialize(self, x):
        x[:] = x.new_full(x.shape,self._v)
# MSE loss
class MSE:
    def __init__(self):
        self._pred = None
        self._target = None

    def apply(self, prediction, target):
        t = target if target.dtype is torch.float32 else target.type(torch.float32)
        p = prediction.data
        p = p if p.dtype is torch.float32 else p.type(torch.float32)
        self._pred = p
        self._target = t
        loss = (p - t).square().mean()*0.5

        delta= self._pred - t
        return loss, delta
    def __call__(self, prediction, target):
        return self.apply(prediction, target)
# BCE loss
class BCE(MSE):
    def __init__(self):
        super().__init__()
        self._eps = 1e-6 #avoid singularity
    def apply(self, prediction, target):
        t = target if target.dtype is torch.float32 else target.type(torch.float32)
        p = prediction.data
        p = p if p.dtype is torch.float32 else p.type(torch.float32)
        p=sigmoid.forward(p)
        self._pred = p
        self._target = t
        inter=t * ((p + self._eps).log()) + (1. - t) * ((1 - p + self._eps).log())
        loss = - inter.mean()
        delta= self._pred - t
        return loss, delta
    def __call__(self, prediction, target):

        return self.apply(prediction, target)
class Activation:
    def forward(self, x):
        raise NotImplementedError

    def derivative(self, x):
        raise NotImplementedError

    def __call__(self, *inputs):
        return self.forward(*inputs)


class Linear(Activation):
    def forward(self, x):
        return x

    def derivative(self, x):
        return x.new_ones(x.shape)


class ReLU(Activation):
    def forward(self, x):
        return x.maximum(torch.tensor([0]))

    def derivative(self, x):
        return where_one_zero(x, 1.0,0.0,x>0)
    
class Tanh(Activation):
    def forward(self, x):
        return (10*x).tanh()

    def derivative(self, x):
        return (1. - (10*x).tanh().square())*10


class Sigmoid(Activation):
    def forward(self, x):
        return 1./(1.+(-x).exp())

    def derivative(self, x):
        f = self.forward(x)
        return f*(1.-f)
#SGD optimizer
class SGD:
    def __init__(self, params, lr):
        self._params = params
        self._lr = lr
        self.vars = []
        self.grads = []
        for layer_p in self._params.values():
            for p_name in layer_p["paras"].keys():
                self.vars.append(layer_p["paras"][p_name])
                self.grads.append(layer_p["grads"][p_name])

    def step(self):
        for var, grad in zip(self.vars, self.grads):
            var -= self._lr * grad
#Adam optimizer
class Adam(SGD):
    def __init__(self, params, lr=0.01, betas=(0.9, 0.999), eps=1e-07):
        super().__init__(params=params, lr=lr)
        self._betas = betas
        self._eps = eps
        self._m = [v.new_full(v.shape,0.0) for v in self.vars]
        self._v = [v.new_full(v.shape,0.0)  for v in self.vars]

    def step(self):
        b1, b2 = self._betas
        b1_crt, b2_crt = b1, b2
        for var, grad, m, v in zip(self.vars, self.grads, self._m, self._v):
            m[:] = b1 * m + (1. - b1) * grad
            v[:] = b2 * v + (1. - b2) * grad.square()
            b1_crt, b2_crt = b1_crt * b1, b2_crt * b2 
            m_crt = m / (1. - b1_crt)
            v_crt = v / (1. - b2_crt)
            var -= self._lr * m_crt /((v_crt + self._eps).sqrt_())

relu = ReLU()
tanh = Tanh()
sigmoid = Sigmoid()