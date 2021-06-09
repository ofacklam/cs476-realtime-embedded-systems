import torch
torch.set_grad_enabled(False)
import nn_lib as nn
#initialize NN for classification task
relu = nn.ReLU()
tanh = nn.Tanh()
sigmoid = nn.Sigmoid()
class Net(nn.MiniNN):
    def __init__(self):
        super().__init__()
        w_ini=nn.RandomUniform()
        self.l1 = nn.Layer_stack(n_in=2, n_out=10, activation=tanh,w_initializer=w_ini)
        self.l2 = nn.Layer_stack(n_in=10, n_out=10, activation=tanh,w_initializer=w_ini)
        self.out = nn.Layer_stack(n_in=10, n_out=1, activation=sigmoid)
    def forward(self, x):
        x1 = self.l1(x)
        x2 = self.l2(x1)
        o=self.out(x2)
        return o
class NetSeq(nn.MiniNN):
    def __init__(self):
        super().__init__()
        w_ini_relu_27=nn.RandomUniform(-0.67,0.67)
        w_ini_relu_50=nn.RandomUniform(-0.49,0.49)
        w_ini_tanh_50=nn.RandomUniform(-0.35,0.35)
        w_ini_sig_50=nn.RandomUniform(-1.39,1.39)
        w_ini_sig_26=nn.RandomUniform(-1.9,1.9)
        self.seq_layer= self.sequential(
        nn.Layer_stack(n_in=2, n_out=25, activation=relu,w_initializer=w_ini_relu_27,b_initializer=None),
        nn.Layer_stack(n_in=25, n_out=25, activation=tanh,w_initializer=w_ini_tanh_50,b_initializer=None),
        nn.Layer_stack(n_in=25, n_out=25, activation=relu,w_initializer=w_ini_relu_50,b_initializer=None),
        nn.Layer_stack(n_in=25, n_out=1, activation=sigmoid,w_initializer=w_ini_sig_26)
        )
    def forward(self, x):
        o=self.seq_layer(x)
        return o
def accuracy(predictions, labels):
    p, l = predictions.data.type(torch.float32), labels.type(torch.float32)
    acc = torch.empty(p.shape)
    acc_zero=acc.zero_()
    acc_one=acc.new_full(acc.shape, 1.0)
    return acc_one.where(p == l,acc_zero).mean()


data_log=[]
test_log=[]
test_error=[]
for i in range(10):  
  r=0.399043;
  mini_batch_size=100
  net = NetSeq()
  opt = nn.SGD(net.params, lr=1e-4)
  loss_fn =nn.MSE()
  x = torch.empty((1000, 2)) 
  x=x.uniform_()
  ct_c=torch.tensor([(0.5,0.5)])
  y = nn.where_one_zero((x-ct_c).norm(dim=1),1,0,((x-ct_c).norm(dim=1))<r)
  #first dim: internal, second dim: external
  y_l=y.unsqueeze(1)
  y=y.unsqueeze(1)
  train_log=()

  for epoch in range(100):
    acc_loss=0
    for b in range(0, x.shape[0],mini_batch_size):
        x_b=x.narrow(0, b, mini_batch_size)
        y_b=y.narrow(0, b, mini_batch_size)
        o_b = net.forward(x_b)
        loss,delta = loss_fn(o_b, y_b)
        net.backpass(delta)
        opt.step()
        acc_loss+=loss
    o = net.forward(x)
    loss,delta= loss_fn(o, y)
    net.backpass(delta)
    opt.step()

    op_shot=nn.where_one_zero(o.data,0,1,o.data<0.5)
    acc=accuracy(op_shot, y_l)
    #log the loss and accuracy of each epoch including the final loop
    train_log=train_log+((loss,acc),)

    #print("Epoch: %i | loss: %.5f | acc: %.2f" % (epoch, loss, acc))
  data_log+=[train_log]
  x_t = torch.empty((1000, 2)) 
  x_t=x_t.uniform_()
  o = net.forward(x_t)
  loss,delta= loss_fn(o, y)
  y_t = nn.where_one_zero((x_t-ct_c).norm(dim=1),1,0,((x_t-ct_c).norm(dim=1))<r)
  y_t=y_t.unsqueeze(1)

  o_shot=nn.where_one_zero(o.data,0,1,o.data<0.5)
  acc =accuracy(o_shot, y_t)
  test_log+=[acc]
  test_error+=[loss]
  #log and print
  print("The average loss of 1000 train samples of final epoch in round %i is %.5f"%(i+1,train_log[-1][0]))    
  print("The accuracy of final epoch of train in round %i is %.5f"%(i+1,train_log[-1][1]))
  print("The average loss of test of round %i is %.5f"%(i+1,loss))
  print("The accuracy of test of round %i is %.5f"%(i+1,acc))

data_test = torch.tensor(test_log, dtype=torch.float32)
test_error= torch.tensor(test_error, dtype=torch.float32)
print("mean of loss:%.5f(%.5f),mean of accuracy:%.5f(%.5f)"%(test_error.mean(),test_error.std(),data_test.mean(),data_test.std()))