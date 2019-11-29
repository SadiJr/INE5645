import pandas as pd
import sklearn
from sklearn import svm, preprocessing
"""
codlegislatura,datemissao,idedocumento,idecadastro,indtipodocumento,nucarteiraparlamentar,nudeputadoid,nulegislatura,numano,
numespecificacaosubcota,numlote,nummes,numparcela,numressarcimento,numsubcota,sgpartido,sguf,txnomeparlamentar,txtcnpjcpf,
txtdescricao,txtdescricaoespecificacao,txtfornecedor,txtnumero,txtpassageiro,txttrecho,vlrdocumento,vlrglosa,vlrliquido,vlrrestituicao

"""

fields = ['vlrliquido', 'sgpartido', 'numano', 'nummes', 'codlegislatura', 'nucarteiraparlamentar']
rows = 10000

df = pd.read_csv('dataset.csv', sep=',', skipinitialspace=True, usecols=fields, nrows= rows).dropna()
df = sklearn.utils.shuffle(df)



party = {'DEM':1, 'PODE':2, 'AVANTE':3, 'PT':4,  'PR':5, 'MDB':6, 'PRB':7, 'PP':8, 'PDT':9, 'PSB':10, 'PTB':11, 'PPS':12,
 'PSD':13, 'PCdoB':14, 'PROS':15, 'PSC':16, 'PV':17, 'PSDB':18, 'SD':19, 'PATRI':20, 'PHS':21, 'PSL':22, 'PSOL':23,
 'REDE':24, 'LID.GOV-CD':25, 'LID.GOV-CN':26, 'PRP':27, 'SDD':28, 'PPL':29, 'PTN':30, 'PRTB':31, 'PMDB':32,
 'PMN':33, 'PTdoB':34, 'S.PART.':35, 'PSDC':36, 'PEN':37, 'PTC':38, 'ATHOS AVELINO':39,
 'FERNANDO ESTIMA':40, 'JOÃO FONTES':41, 'LUIZ MOREIRA':42, 'PROFESSOR LUIZINHO':43,}


df['sgpartido'] = df['sgpartido'].map(party)

#hpt = pd.DataFrame()
	
test_size = 200

X = df.drop("vlrliquido",axis=1).values
X = preprocessing.scale(X)
y = df["vlrliquido"].values 


X_train = X[:-test_size]
y_train = y[:-test_size]

X_test = X[-test_size:]
y_test = y[-test_size:]



clf = svm.SVR()

clf.fit(X_train, y_train)
print(clf.score(X_test, y_test))
#for part in df:
#	print(df['sgpartido'].unique())


#for valor in df:
#	vlrliquido+= df['vlrliquido']




#print(df.head(10))